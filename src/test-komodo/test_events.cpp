#include <gtest/gtest.h>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <boost/filesystem.hpp>
#include <komodo_structs.h>

int32_t komodo_faststateinit(struct komodo_state *sp,const char *fname,char *symbol,char *dest);
struct komodo_state *komodo_stateptrget(char *base);
extern int32_t KOMODO_EXTERNAL_NOTARIES;

namespace test_events {

uint256 fill_hash(uint8_t val)
{
    std::vector<uint8_t> bin(32);
    for(uint8_t i = 0; i < 32; ++i)
        bin[i] = val;
    return uint256(bin);
}

void write_p_record(std::FILE* fp)
{
    // a pubkey record with 2 keys
    char data[5+1+(33*2)] = {'P', 1, 0, 0, 0, 2}; // public key record identifier with height of 1 plus number of keys plus keys themselves
    memset(&data[6], 1, 33); // 1st key is all 1s
    memset(&data[39], 2, 33); // 2nd key is all 2s
    std::fwrite(data, sizeof(data), 1, fp);
}

void write_p_record_new(std::FILE* fp)
{
    komodo::event_pubkeys evt;
    evt.height = 1;
    evt.num = 2;
    memset(&evt.pubkeys[0], 1, 33);
    memset(&evt.pubkeys[1], 2, 33);
    write_event(evt, fp);
}

void write_n_record(std::FILE* fp)
{
    // a notarized record has
    // 5 byte header 
    // 4 bytes notarized_height
    // 32 bytes notarized hash( blockhash)
    // 32 bytes desttxid
    char data[73] = {'N', 1, 0, 0, 0, 2, 0, 0, 0};
    memset(&data[9], 1, 32); // notarized hash
    memset(&data[41], 2, 32); // desttxid
    std::fwrite(data, sizeof(data), 1, fp);
}
void write_n_record_new(std::FILE* fp)
{
    komodo::event_notarized evt;
    evt.height = 1;
    evt.notarizedheight = 2;
    evt.blockhash = fill_hash(1);
    evt.desttxid = fill_hash(2);
    write_event(evt, fp);
}

void write_m_record(std::FILE* fp)
{
    // a notarized M record has
    // 5 byte header 
    // 4 bytes notarized_height
    // 32 bytes notarized hash( blockhash)
    // 32 bytes desttxid
    // 32 bytes MoM
    // 4 bytes MoMdepth
    char data[109] = {'M', 1, 0, 0, 0, 3, 0, 0, 0};
    memset(&data[9], 1, 32); // notarized hash
    memset(&data[41], 2, 32); // desttxid
    memset(&data[73], 3, 32); // MoM
    memset(&data[105], 4, 4); // MoMdepth
    std::fwrite(data, sizeof(data), 1, fp);
}
void write_m_record_new(std::FILE* fp)
{
    komodo::event_notarized evt;
    evt.height = 1;
    evt.notarizedheight = 3;
    std::vector<unsigned char> bin(32);
    evt.blockhash = fill_hash(1);
    evt.desttxid = fill_hash(2);
    evt.MoM = fill_hash(3);
    evt.MoMdepth = 0x04040404;
    write_event(evt, fp);
}

void write_u_record(std::FILE* fp)
{
    // a U record has
    // 5 byte header 
    // 1 byte n and 1 byte nid
    // 8 bytes mask
    // 32 bytes hash
    char data[47] = {'U', 1, 0, 0, 0, 'N', 'I'};
    memset(&data[7], 1, 8); // mask
    memset(&data[15], 2, 32); // hash
    std::fwrite(data, sizeof(data), 1, fp);
}
void write_u_record_new(std::FILE* fp)
{
    komodo::event_u evt(1);
    evt.n = 'N';
    evt.nid = 'I';
    memset(&evt.mask[0], 1, 8);
    memset(&evt.hash[0], 2, 32);
    write_event(evt, fp);
}

void write_k_record(std::FILE* fp)
{
    // a K record has
    // 5 byte header 
    // 4 bytes height
    char data[9] = {'K', 1, 0, 0, 0 };
    memset(&data[5], 1, 4); // height
    std::fwrite(data, sizeof(data), 1, fp);
}
void write_k_record_new(std::FILE* fp)
{
    komodo::event_kmdheight evt(1);
    evt.kheight = 0x01010101;
    write_event(evt, fp);
}

void write_t_record(std::FILE* fp)
{
    // a T record has
    // 5 byte header 
    // 4 bytes height
    // 4 bytes timestamp
    char data[13] = {'T', 1, 0, 0, 0 };
    memset(&data[5], 1, 4); // height
    memset(&data[9], 2, 4); // timestamp
    std::fwrite(data, sizeof(data), 1, fp);
}
void write_t_record_new(std::FILE* fp)
{
    komodo::event_kmdheight evt(1);
    evt.kheight = 0x01010101;
    evt.timestamp = 0x02020202;
    write_event(evt, fp);
}

void write_r_record(std::FILE* fp)
{
    // an R record has
    // 5 byte header 
    // 32 byte txid
    // 2 byte v
    // 8 byte ovalue
    // 2 byte olen
    // olen bytes of data
    char data[50] = {'R', 1, 0, 0, 0 };
    memset(&data[5], 1, 32); // txid
    memset(&data[37], 2, 2); // v
    memset(&data[39], 3, 8); // ovalue
    unsigned char olen[2] = {1,0}; // 1
    memcpy(&data[47], olen, 2);
    memset(&data[49], 4, 1);
    std::fwrite(data, sizeof(data), 1, fp);
}
void write_r_record_new(std::FILE* fp)
{
    komodo::event_opreturn evt(1);
    evt.txid = fill_hash(1);
    evt.vout = 0x0202; 
    evt.value = 0x0303030303030303;
    evt.opret = std::vector<uint8_t>{ 0x04 };
    write_event(evt, fp);
}

void write_v_record(std::FILE* fp)
{
    // a V record has
    // 5 byte header 
    // 1 byte counter
    // counter * 4 bytes of data
    char data[146] = {'V', 1, 0, 0, 0};
    memset(&data[5], 35, 1); // counter
    memset(&data[6], 1, 140); // data
    std::fwrite(data, sizeof(data), 1, fp);
}
void write_v_record_new(std::FILE* fp)
{
    komodo::event_pricefeed evt(1);
    evt.num = 35;
    memset(&evt.prices[0], 1, 140);
    write_event(evt, fp);
}

void write_b_record(std::FILE* fp)
{
    char data[] = {'B', 1, 0, 0, 0};
    std::fwrite(data, sizeof(data), 1, fp);
}
void write_b_record_new(std::FILE* fp)
{
    komodo::event_rewind evt(1);
    write_event(evt, fp);
}

template<class T>
bool compare_serialization(const std::string& filename, const T& in)
{
    // read contents of file
    std::ifstream s(filename, std::ios::binary);
    std::vector<char> file_contents((std::istreambuf_iterator<char>(s)), std::istreambuf_iterator<char>());
    // get contents of in
    std::stringstream ss;
    ss << in;
    std::vector<char> in_contents( (std::istreambuf_iterator<char>(ss)), std::istreambuf_iterator<char>()); 
    bool retval = file_contents == in_contents;
    if (!retval)
    {
        if (file_contents.size() != in_contents.size())
        {
            std::cout << "File has " << std::to_string( file_contents.size() ) << " bytes but serialized data has " << in_contents.size() << "\n";
        }
        else
        {
            for(size_t i = 0; i < file_contents.size(); ++i)
            {
                if (file_contents[i] != in_contents[i])
                {
                    std::cout << "Difference at position " << std::to_string(i) 
                            << " " << std::hex << std::setfill('0') << std::setw(2) << (int)file_contents[i] 
                            << " " << std::hex << std::setfill('0') << std::setw(2) << (int)in_contents[i] << "\n";
                    break;
                }
            }
        }
    }
    return retval;
}

bool compare_files(const std::string& file1, const std::string& file2)
{
    std::ifstream f1(file1, std::ifstream::binary|std::ifstream::ate);
    std::ifstream f2(file2, std::ifstream::binary|std::ifstream::ate);

    if (f1.fail() || f2.fail()) {
        std::cerr << "Unable to open file\n";
        return false; //file problem
    }

    if (f1.tellg() != f2.tellg()) {
        std::cerr << "Files not the same size\n";
        return false; //size mismatch
    }

    size_t sz = f1.tellg();
    f1.seekg(0, std::ifstream::beg);
    f2.seekg(0, std::ifstream::beg);
    for(size_t i = 0; i < sz; ++i)
    {
        char a;
        f1.read(&a, 1);
        char b;
        f2.read(&b, 1);
        if (a != b)
        {
            std::cerr << "Data mismatch at position " << i << std::endl;
            return false;
        }
    }
    return true;
}

void clear_state(const char* symbol)
{
    komodo_state* state = komodo_stateptrget((char*)symbol);
    EXPECT_NE(state, nullptr);
    state->events.clear();
}
/****
 * The main purpose of this test is to verify that
 * state files created continue to be readable despite logic
 * changes. Files need to be readable. The record format should 
 * not change without hardfork protection
 */
TEST(test_events, komodo_faststateinit_test)
{
    char symbol[] = "TST";
    strcpy(ASSETCHAINS_SYMBOL, symbol);
    KOMODO_EXTERNAL_NOTARIES = 1;

    clear_state(symbol);

    boost::filesystem::path temp = boost::filesystem::unique_path();
    boost::filesystem::create_directories(temp);
    try
    {
        // NOTE: header contains a 5 byte header that is make up of
        // an 8 bit identifier (i.e. 'P', 'N', etc.)
        // plus a 32 bit height. Everything else is record specific
        // pubkey record
        {
            // create a binary file that should be readable by komodo
            const std::string full_filename = (temp / "kstate.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_p_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = nullptr;
            // attempt to read the file
            int32_t result = komodo_faststateinit( state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            /* old way
            // The first and second event should be pub keys
            EXPECT_EQ(state->Komodo_numevents, 1);
            komodo_event* ev1 = state->Komodo_events[0];
            EXPECT_EQ(ev1->height, 1);
            EXPECT_EQ(ev1->type, 'P');
            */
            // check that the new way is the same
            EXPECT_EQ(state->events.size(), 1);
            komodo::event_pubkeys& ev2 = 
                    static_cast<komodo::event_pubkeys&>( *state->events.front() );
            EXPECT_EQ(ev2.height, 1);
            EXPECT_EQ(ev2.type, komodo::komodo_event_type::EVENT_PUBKEYS);
            // the serialized version should match the input
            EXPECT_TRUE(compare_serialization(full_filename, ev2));
        }
        // notarized record
        {
            const std::string full_filename = (temp / "notarized.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_n_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit( state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            /* old way
            EXPECT_EQ(state->Komodo_numevents, 2);
            komodo_event* ev = state->Komodo_events[1];
            EXPECT_EQ(ev->height, 1);
            EXPECT_EQ(ev->type, 'N');
            */
            // check that the new way is the same
            EXPECT_EQ(state->events.size(), 2);
            komodo::event_notarized& ev2 = 
                    static_cast<komodo::event_notarized&>( *(*(++state->events.begin())) );
            EXPECT_EQ(ev2.height, 1);
            EXPECT_EQ(ev2.type, komodo::komodo_event_type::EVENT_NOTARIZED);
            // the serialized version should match the input
            EXPECT_TRUE(compare_serialization(full_filename, ev2));
        }
        // notarized M record
        {
            const std::string full_filename = (temp / "notarized.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_m_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            /* old way
            EXPECT_EQ(state->Komodo_numevents, 3);
            komodo_event* ev = state->Komodo_events[2];
            EXPECT_EQ(ev->height, 1);
            EXPECT_EQ(ev->type, 'N'); // code converts "M" to "N"
            */
            // check that the new way is the same
            EXPECT_EQ(state->events.size(), 3);
            auto itr = state->events.begin();
            std::advance(itr, 2);
            komodo::event_notarized& ev2 = static_cast<komodo::event_notarized&>( *(*(itr)) );
            EXPECT_EQ(ev2.height, 1);
            EXPECT_EQ(ev2.type, komodo::komodo_event_type::EVENT_NOTARIZED);
            // the serialized version should match the input
            EXPECT_TRUE(compare_serialization(full_filename, ev2));
        }
        // record type "U" (deprecated)
        {
            const std::string full_filename = (temp / "type_u.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_u_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            /* old way
            EXPECT_EQ(state->Komodo_numevents, 3); // does not get added to state
            */
            // check that the new way is the same
            EXPECT_EQ(state->events.size(), 3);
            auto itr = state->events.begin();
            // this does not get added to state, so we need to serialize the object just
            // to verify serialization works as expected
            komodo::event_u ev2;
            ev2.height = 1;
            ev2.n = 'N';
            ev2.nid = 'I';
            memset(ev2.mask, 1, 8);
            memset(ev2.hash, 2, 32);
            EXPECT_TRUE(compare_serialization(full_filename, ev2));
        }
        // record type K (KMD height)
        {
            const std::string full_filename = (temp / "kmdtype.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_k_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            /* old way
            EXPECT_EQ(state->Komodo_numevents, 4);
            komodo_event* ev = state->Komodo_events[3];
            EXPECT_EQ(ev->height, 1);
            EXPECT_EQ(ev->type, 'K');            
            */
            // check that the new way is the same
            EXPECT_EQ(state->events.size(), 4);
            auto itr = state->events.begin();
            std::advance(itr, 3);
            komodo::event_kmdheight& ev2 = static_cast<komodo::event_kmdheight&>( *(*(itr)) );
            EXPECT_EQ(ev2.height, 1);
            EXPECT_EQ(ev2.type, komodo::komodo_event_type::EVENT_KMDHEIGHT);
            // the serialized version should match the input
            EXPECT_TRUE(compare_serialization(full_filename, ev2));
        }
        // record type T (KMD height with timestamp)
        {
            const std::string full_filename = (temp / "kmdtypet.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_t_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            /* old way
            EXPECT_EQ(state->Komodo_numevents, 5);
            komodo_event* ev = state->Komodo_events[4];
            EXPECT_EQ(ev->height, 1);
            EXPECT_EQ(ev->type, 'K'); // changed from T to K
            */
            // check that the new way is the same
            EXPECT_EQ(state->events.size(), 5);
            auto itr = state->events.begin();
            std::advance(itr, 4);
            komodo::event_kmdheight& ev2 = static_cast<komodo::event_kmdheight&>( *(*(itr)) );
            EXPECT_EQ(ev2.height, 1);
            EXPECT_EQ(ev2.type, komodo::komodo_event_type::EVENT_KMDHEIGHT);
            // the serialized version should match the input
            EXPECT_TRUE(compare_serialization(full_filename, ev2));
        }
        // record type R (opreturn)
        {
            const std::string full_filename = (temp / "kmdtypet.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_r_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            /* old way
            EXPECT_EQ(state->Komodo_numevents, 6);
            komodo_event* ev = state->Komodo_events[5];
            EXPECT_EQ(ev->height, 1);
            EXPECT_EQ(ev->type, 'R');
            */
            // check that the new way is the same
            EXPECT_EQ(state->events.size(), 6);
            auto itr = state->events.begin();
            std::advance(itr, 5);
            komodo::event_opreturn& ev2 = static_cast<komodo::event_opreturn&>( *(*(itr)) );
            EXPECT_EQ(ev2.height, 1);
            EXPECT_EQ(ev2.type, komodo::komodo_event_type::EVENT_OPRETURN);
            // the serialized version should match the input
            EXPECT_TRUE(compare_serialization(full_filename, ev2));
        }
        // record type V
        {
            const std::string full_filename = (temp / "kmdtypet.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_v_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            /* old way
            EXPECT_EQ(state->Komodo_numevents, 7);
            komodo_event* ev = state->Komodo_events[6];
            EXPECT_EQ(ev->height, 1);
            EXPECT_EQ(ev->type, 'V');
            */
            // check that the new way is the same
            EXPECT_EQ(state->events.size(), 7);
            auto itr = state->events.begin();
            std::advance(itr, 6);
            komodo::event_pricefeed& ev2 = static_cast<komodo::event_pricefeed&>( *(*(itr)) );
            EXPECT_EQ(ev2.height, 1);
            EXPECT_EQ(ev2.type, komodo::komodo_event_type::EVENT_PRICEFEED);
            // the serialized version should match the input
            EXPECT_TRUE(compare_serialization(full_filename, ev2));
        }
        // record type B (rewind)
        {
            const std::string full_filename = (temp / "kmdtypeb.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_b_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            // NOTE: B records are not read in. Unsure if this is on purpose or an oversight
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            /* old way
            EXPECT_EQ(state->Komodo_numevents, 7);
            komodo_event* ev = state->Komodo_events[6];
            EXPECT_EQ(ev->height, 1);
            EXPECT_EQ(ev->type, 'B');
            */
            // check that the new way is the same
            EXPECT_EQ(state->events.size(), 7);
            /*
            auto itr = state->events.begin();
            std::advance(itr, 6);
            std::shared_ptr<komodo::event_rewind> ev2 = std::dynamic_pointer_cast<komodo::event_rewind>( *(itr) );
            EXPECT_NE(ev2, nullptr);
            EXPECT_EQ(ev2->height, 1);
            EXPECT_EQ(ev2->type, komodo::komodo_event_type::EVENT_REWIND);
            // the serialized version should match the input
            EXPECT_TRUE(compare_serialization(full_filename, ev2));
            */
        }        
        // all together in 1 file
        {
            const std::string full_filename = (temp / "combined_state.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_p_record(fp);
            write_n_record(fp);
            write_m_record(fp);
            write_u_record(fp);
            write_k_record(fp);
            write_t_record(fp);
            write_r_record(fp);
            write_v_record(fp);
            std::fclose(fp);
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit( state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            /* old way
            EXPECT_EQ(state->Komodo_numevents, 14);
            komodo_event* ev1 = state->Komodo_events[7];
            EXPECT_EQ(ev1->height, 1);
            EXPECT_EQ(ev1->type, 'P');
            */
            // check that the new way is the same
            EXPECT_EQ(state->events.size(), 14);
            auto itr = state->events.begin();
            std::advance(itr, 7);
            {
                EXPECT_EQ( (**itr).height, 1);
                EXPECT_EQ( (**itr).type, komodo::komodo_event_type::EVENT_PUBKEYS);
                itr++;
            }
            {
                EXPECT_EQ( (**itr).height, 1);
                EXPECT_EQ( (**itr).type, komodo::komodo_event_type::EVENT_NOTARIZED);
                itr++;
            }
            {
                EXPECT_EQ( (**itr).height, 1);
                EXPECT_EQ( (**itr).type, komodo::komodo_event_type::EVENT_NOTARIZED);
                itr++;
            }
            {
                EXPECT_EQ( (**itr).height, 1);
                EXPECT_EQ( (**itr).type, komodo::komodo_event_type::EVENT_KMDHEIGHT);
                itr++;
            }
            {
                EXPECT_EQ( (**itr).height, 1);
                EXPECT_EQ( (**itr).type, komodo::komodo_event_type::EVENT_KMDHEIGHT);
                itr++;
            }
            {
                EXPECT_EQ( (**itr).height, 1);
                EXPECT_EQ( (**itr).type, komodo::komodo_event_type::EVENT_OPRETURN);
                itr++;
            }
            {
                EXPECT_EQ( (**itr).height, 1);
                EXPECT_EQ( (**itr).type, komodo::komodo_event_type::EVENT_PRICEFEED);
                itr++;
            }
        }
    } 
    catch(...)
    {
        FAIL() << "Exception thrown";
    }
    boost::filesystem::remove_all(temp);
}

TEST(test_events, komodo_faststateinit_test_kmd)
{
    // Nothing should be added to events if this is the komodo chain

    char symbol[] = "KMD";
    ASSETCHAINS_SYMBOL[0] = 0;
    KOMODO_EXTERNAL_NOTARIES = 0;

    clear_state(symbol);

    boost::filesystem::path temp = boost::filesystem::unique_path();
    boost::filesystem::create_directories(temp);
    try
    {
        // NOTE: header contains a 5 byte header that is make up of
        // an 8 bit identifier (i.e. 'P', 'N', etc.)
        // plus a 32 bit height. Everything else is record specific
        // pubkey record
        {
            // create a binary file that should be readable by komodo
            const std::string full_filename = (temp / "kstate.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_p_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = nullptr;
            // attempt to read the file
            int32_t result = komodo_faststateinit( state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            EXPECT_EQ(state->events.size(), 0);
        }
        // notarized record
        {
            const std::string full_filename = (temp / "notarized.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_n_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit( state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            EXPECT_EQ(state->events.size(), 0);
        }
        // notarized M record
        {
            const std::string full_filename = (temp / "notarized.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_m_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            EXPECT_EQ(state->events.size(), 0);
        }
        // record type "U" (deprecated)
        {
            const std::string full_filename = (temp / "type_u.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_u_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            EXPECT_EQ(state->events.size(), 0);
        }
        // record type K (KMD height)
        {
            const std::string full_filename = (temp / "kmdtype.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_k_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            EXPECT_EQ(state->events.size(), 0);
        }
        // record type T (KMD height with timestamp)
        {
            const std::string full_filename = (temp / "kmdtypet.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_t_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            EXPECT_EQ(state->events.size(), 0);
        }
        // record type R (opreturn)
        {
            const std::string full_filename = (temp / "kmdtypet.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_r_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            EXPECT_EQ(state->events.size(), 0);
        }
        // record type V
        {
            const std::string full_filename = (temp / "kmdtypet.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_v_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            EXPECT_EQ(state->events.size(), 0);
        }
        // record type B (rewind)
        {
            const std::string full_filename = (temp / "kmdtypeb.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_b_record(fp);
            std::fclose(fp);
            // verify file still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            // NOTE: B records are not read in. Unsure if this is on purpose or an oversight
            int32_t result = komodo_faststateinit(state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            EXPECT_EQ(state->events.size(), 0);
        }        
        // all together in 1 file
        {
            const std::string full_filename = (temp / "combined_state.tmp").string();
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_p_record(fp);
            write_n_record(fp);
            write_m_record(fp);
            write_u_record(fp);
            write_k_record(fp);
            write_t_record(fp);
            write_r_record(fp);
            write_v_record(fp);
            std::fclose(fp);
            // attempt to read the file
            komodo_state* state = komodo_stateptrget((char*)symbol);
            EXPECT_NE(state, nullptr);
            char* dest = (char*)"123456789012345";
            // attempt to read the file
            int32_t result = komodo_faststateinit( state, full_filename.c_str(), symbol, dest);
            // compare results
            EXPECT_EQ(result, 1);
            EXPECT_EQ(state->events.size(), 0);
        }
    } 
    catch(...)
    {
        FAIL() << "Exception thrown";
    }
    boost::filesystem::remove_all(temp);
}

TEST(test_events, write_test)
{
    // test serialization of the different event records
    char symbol[] = "TST";
    strcpy(ASSETCHAINS_SYMBOL, symbol);
    KOMODO_EXTERNAL_NOTARIES = 1;

    clear_state(symbol);

    boost::filesystem::path temp = boost::filesystem::unique_path();
    boost::filesystem::create_directories(temp);

    const std::string full_filename = (temp / "kstate.tmp").string();
    const std::string full_filename2 = (temp / "kstate2.tmp").string();
    try
    {
        {
            // old way
            std::FILE* fp = std::fopen(full_filename.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_b_record(fp);
            write_k_record(fp);
            write_m_record(fp);
            write_n_record(fp);
            write_p_record(fp);
            write_r_record(fp);
            write_t_record(fp);
            write_u_record(fp);
            write_v_record(fp);
            std::fclose(fp);
            // verify files still exists
            EXPECT_TRUE(boost::filesystem::exists(full_filename));
        }
        {
            // new way
            std::FILE* fp = std::fopen(full_filename2.c_str(), "wb+");
            EXPECT_NE(fp, nullptr);
            write_b_record_new(fp);
            write_k_record_new(fp);
            write_m_record_new(fp);
            write_n_record_new(fp);
            write_p_record_new(fp);
            write_r_record_new(fp);
            write_t_record_new(fp);
            write_u_record_new(fp);
            write_v_record_new(fp);
            std::fclose(fp);
            EXPECT_TRUE(boost::filesystem::exists(full_filename2));
            // the two files should be binarily equal
            EXPECT_TRUE( compare_files(full_filename, full_filename2) );
        }
    }
    catch(...)
    {
        FAIL() << "Exception thrown";
    }
    boost::filesystem::remove_all(temp);
    //std::cout << full_filename << " " << full_filename2 << "\n";
}

} // namespace test_events