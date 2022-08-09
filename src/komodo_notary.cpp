/******************************************************************************
 * Copyright © 2014-2019 The SuperNET Developers.                             *
 *                                                                            *
 * See the AUTHORS, DEVELOPER-AGREEMENT and LICENSE files at                  *
 * the top-level directory of this distribution for the individual copyright  *
 * holder information and the developer policies on copyright and licensing.  *
 *                                                                            *
 * Unless otherwise agreed in a custom licensing agreement, no part of the    *
 * SuperNET software, including this file may be copied, modified, propagated *
 * or distributed except according to the terms contained in the LICENSE file *
 *                                                                            *
 * Removal or modification of this copyright notice is prohibited.            *
 *                                                                            *
 ******************************************************************************/
#include "komodo_notary.h"
#include "komodo_extern_globals.h"
#include "komodo.h" // komodo_stateupdate()
#include "komodo_structs.h" // KOMODO_NOTARIES_HARDCODED
#include "komodo_utils.h" // komodo_stateptr

const char *Notaries_genesis[][2] =
{
    { "jl777_testA", "03b7621b44118017a16043f19b30cc8a4cfe068ac4e42417bae16ba460c80f3828" },
    { "jl777_testB", "02ebfc784a4ba768aad88d44d1045d240d47b26e248cafaf1c5169a42d7a61d344" },
    { "pondsea_SH", "02209073bc0943451498de57f802650311b1f12aa6deffcd893da198a544c04f36" },
    { "crackers_EU", "0340c66cf2c41c41efb420af57867baa765e8468c12aa996bfd816e1e07e410728" },
    { "pondsea_EU", "0225aa6f6f19e543180b31153d9e6d55d41bc7ec2ba191fd29f19a2f973544e29d" },
    { "locomb_EU", "025c6d26649b9d397e63323d96db42a9d3caad82e1d6076970efe5056c00c0779b" },
    { "fullmoon_AE", "0204a908350b8142698fdb6fabefc97fe0e04f537adc7522ba7a1e8f3bec003d4a" },
    { "movecrypto_EU", "021ab53bc6cf2c46b8a5456759f9d608966eff87384c2b52c0ac4cc8dd51e9cc42" },
    { "badass_EU", "0209d48554768dd8dada988b98aca23405057ac4b5b46838a9378b95c3e79b9b9e" },
    { "crackers_NA", "029e1c01131974f4cd3f564cc0c00eb87a0f9721043fbc1ca60f9bd0a1f73f64a1" },
    { "proto_EU", "03681ffdf17c8f4f0008cefb7fa0779c5e888339cdf932f0974483787a4d6747c1" }, // 10
    { "jeezy_EU", "023cb3e593fb85c5659688528e9a4f1c4c7f19206edc7e517d20f794ba686fd6d6" },
    { "farl4web_EU", "035caa40684ace968677dca3f09098aa02b70e533da32390a7654c626e0cf908e1" },
    { "nxtswe_EU", "032fb104e5eaa704a38a52c126af8f67e870d70f82977e5b2f093d5c1c21ae5899" },
    { "traderbill_EU", "03196e8de3e2e5d872f31d79d6a859c8704a2198baf0af9c7b21e29656a7eb455f" },
    { "vanbreuk_EU", "024f3cad7601d2399c131fd070e797d9cd8533868685ddbe515daa53c2e26004c3" }, // 15
    { "titomane_EU", "03517fcac101fed480ae4f2caf775560065957930d8c1facc83e30077e45bdd199" },
    { "supernet_AE", "029d93ef78197dc93892d2a30e5a54865f41e0ca3ab7eb8e3dcbc59c8756b6e355" },
    { "supernet_EU", "02061c6278b91fd4ac5cab4401100ffa3b2d5a277e8f71db23401cc071b3665546" },
    { "supernet_NA", "033c073366152b6b01535e15dd966a3a8039169584d06e27d92a69889b720d44e1" },
    { "yassin_EU", "033fb7231bb66484081952890d9a03f91164fb27d392d9152ec41336b71b15fbd0" }, // 20
    { "durerus_EU", "02bcbd287670bdca2c31e5d50130adb5dea1b53198f18abeec7211825f47485d57" },
    { "badass_SH", "026b49dd3923b78a592c1b475f208e23698d3f085c4c3b4906a59faf659fd9530b" },
    { "badass_NA", "02afa1a9f948e1634a29dc718d218e9d150c531cfa852843a1643a02184a63c1a7" },
    { "pondsea_NA", "031bcfdbb62268e2ff8dfffeb9ddff7fe95fca46778c77eebff9c3829dfa1bb411" },
    { "rnr_EU", "0287aa4b73988ba26cf6565d815786caf0d2c4af704d7883d163ee89cd9977edec" },
    { "crackers_SH", "02313d72f9a16055737e14cfc528dcd5d0ef094cfce23d0348fe974b6b1a32e5f0" },
    { "grewal_SH", "03212a73f5d38a675ee3cdc6e82542a96c38c3d1c79d25a1ed2e42fcf6a8be4e68" },
    { "polycryptoblock_NA", "02708dcda7c45fb54b78469673c2587bfdd126e381654819c4c23df0e00b679622" },
    { "titomane_NA", "0387046d9745414fb58a0fa3599078af5073e10347e4657ef7259a99cb4f10ad47" },
    { "titomane_AE", "03cda6ca5c2d02db201488a54a548dbfc10533bdc275d5ea11928e8d6ab33c2185" },
    { "kolo_EU", "03f5c08dadffa0ffcafb8dd7ffc38c22887bd02702a6c9ac3440deddcf2837692b" },
    { "artik_NA", "0224e31f93eff0cc30eaf0b2389fbc591085c0e122c4d11862c1729d090106c842" },
    { "eclips_EU", "0339369c1f5a2028d44be7be6f8ec3b907fdec814f87d2dead97cab4edb71a42e9" },
    { "titomane_SH", "035f49d7a308dd9a209e894321f010d21b7793461b0c89d6d9231a3fe5f68d9960" },
};

// statics used within this .cpp for caching purposes
static int didinit; // see komodo_init
static uint8_t kmd_pubkeys[NUM_KMD_SEASONS][64][33]; // see komodo_notaries
static int32_t hwmheight; // highest height ever passed to komodo_notariesinit
static int32_t hadnotarization; // used in komodo_dpowconfs
static bool didinit_NOTARIES[NUM_KMD_SEASONS]; // komodo_notaries()


/****
 * @brief get the kmd season based on height (used on the KMD chain)
 * @param height the chain height
 * @returns the KMD season (returns 0 if the height is above the range)
 */
int32_t getkmdseason(int32_t height)
{
    if ( height <= KMD_SEASON_HEIGHTS[0] )
        return 1;
    for (int32_t i = 1; i < NUM_KMD_SEASONS; i++)
    {
        if ( height <= KMD_SEASON_HEIGHTS[i] && height > KMD_SEASON_HEIGHTS[i-1] )
            return i+1;
    }
    return 0;
}

/****
 * @brief get the season based on timestamp (used for alternate chains)
 * @param timestamp the time
 * @returns the KMD season (returns 0 if timestamp is above the range)
 */
int32_t getacseason(uint32_t timestamp)
{
    if ( timestamp <= KMD_SEASON_TIMESTAMPS[0] )
        return 1;
    for (int32_t i = 1; i < NUM_KMD_SEASONS; i++)
    {
        if ( timestamp <= KMD_SEASON_TIMESTAMPS[i] && timestamp > KMD_SEASON_TIMESTAMPS[i-1] )
            return i+1;
    }
    return 0;
}

/*****
 * 2 Helpers for unit tests that reset statics (among other things)
 * DO NOT USE for anything other than unit tests
 */
void undo_init_STAKED(); // see notaries_staked.cpp
/*void undo_init_notaries()
{
    undo_init_STAKED();
    memset(didinit_notaries, 0, NUM_KMD_SEASONS * sizeof(bool) );
    if (Pubkeys != nullptr)
    {
        free(Pubkeys);
        Pubkeys = nullptr;
    }
    hwmheight = 0;
    didinit = false;
}*/

/****
 * Calculate the height index (how notaries are stored) based on the height
 * @param height the height
 * @returns the height index
 */
int32_t ht_index_from_height(int32_t height)
{
    int32_t htind = height / KOMODO_ELECTION_GAP;
    if ( htind >= KOMODO_MAXBLOCKS / KOMODO_ELECTION_GAP )
        htind = (KOMODO_MAXBLOCKS / KOMODO_ELECTION_GAP) - 1;
    return htind;
}

/***
 * @brief Given a height or timestamp, get the appropriate notary keys
 * @param[out] pubkeys the results
 * @param[in] height the height
 * @param[in] timestamp the timestamp
 * @returns the number of notaries
 */
int32_t komodo_notaries(uint8_t pubkeys[64][33],int32_t height,uint32_t timestamp)
{
    // calculate timestamp if necessary (only height passed in and non-KMD chain)
    // TODO: check if this logic changed okay 
    if ( ASSETCHAINS_SYMBOL[0] == 0 )   
        timestamp = 0; // For KMD, we always use height
    else if ( timestamp == 0 )
        timestamp = komodo_heightstamp(height); // derive the timestamp from the passed-in height

    // If this chain is not a staked chain, use the normal Komodo logic to determine notaries. 
    // This allows KMD to still sync and use its proper pubkeys for dPoW.
    if ( is_STAKED(ASSETCHAINS_SYMBOL) == 0 )
    {
        int32_t kmd_season = 0;
        if ( ASSETCHAINS_SYMBOL[0] == 0 )
        {
            // This is KMD, use block heights to determine the KMD notary season.. 
            if ( height >= KOMODO_NOTARIES_HARDCODED )
                kmd_season = getkmdseason(height);
        }
        else 
        {
            // This is a non LABS assetchain, use timestamp to detemine notary pubkeys. 
            kmd_season = getacseason(timestamp);
        }
        if ( kmd_season != 0 )
        {
            if ( !didinit_NOTARIES[kmd_season-1] )
            {
                for (int32_t i=0; i<NUM_KMD_NOTARIES; i++) 
                    decode_hex(kmd_pubkeys[kmd_season-1][i],33,(char *)notaries_elected[kmd_season-1][i][1]);
                if ( ASSETCHAINS_PRIVATE != 0 )
                {
                    // this is PIRATE, we need to populate the address array for the notary exemptions. 
                    for (int32_t i = 0; i<NUM_KMD_NOTARIES; i++)
                        pubkey2addr((char *)NOTARY_ADDRESSES[kmd_season-1][i],(uint8_t *)kmd_pubkeys[kmd_season-1][i]);
                }
                didinit_NOTARIES[kmd_season-1] = true;
            }
            memcpy(pubkeys,kmd_pubkeys[kmd_season-1],NUM_KMD_NOTARIES * 33);
            return(NUM_KMD_NOTARIES);
        }
    }
    else if ( timestamp != 0 )
    { 
        // here we can activate our pubkeys for LABS chains everythig is in notaries_staked.cpp
        int32_t staked_era; int8_t numSN;
        uint8_t staked_pubkeys[64][33];
        staked_era = STAKED_era(timestamp);
        numSN = numStakedNotaries(staked_pubkeys,staked_era);
        memcpy(pubkeys,staked_pubkeys,numSN * 33);
        return(numSN);
    }

    if ( Pubkeys == nullptr )
    {
        komodo_init(height);
    }
    int32_t htind = ht_index_from_height(height);
    std::lock_guard<std::mutex> lock(komodo_mutex);
    int32_t n = Pubkeys[htind].numnotaries;
    uint64_t mask = 0;
    knotary_entry *kp;
    knotary_entry *tmp;
    HASH_ITER(hh,Pubkeys[htind].Notaries,kp,tmp)
    {
        if ( kp->notaryid < n )
        {
            mask |= (1LL << kp->notaryid);
            memcpy(pubkeys[kp->notaryid],kp->pubkey,33);
        } 
        else 
            printf("illegal notaryid.%d vs n.%d\n",kp->notaryid,n);
    }
    if ( (n < 64 && mask == ((1LL << n)-1)) || (n == 64 && mask == 0xffffffffffffffffLL) )
        return n;
    printf("error retrieving notaries ht.%d got mask.%llx for n.%d\n",height,(long long)mask,n);
    return -1;
}

int32_t komodo_electednotary(int32_t *numnotariesp,uint8_t *pubkey33,int32_t height,uint32_t timestamp)
{
    int32_t i,n; uint8_t pubkeys[64][33];
    n = komodo_notaries(pubkeys,height,timestamp);
    *numnotariesp = n;
    for (i=0; i<n; i++)
    {
        if ( memcmp(pubkey33,pubkeys[i],33) == 0 )
            return(i);
    }
    return(-1);
}

int32_t komodo_ratify_threshold(int32_t height,uint64_t signedmask)
{
    int32_t numnotaries, i, wt = 0;
    int32_t htind = ht_index_from_height(height);
    numnotaries = Pubkeys[htind].numnotaries;
    for (i=0; i<numnotaries; i++)
        if ( ((1LL << i) & signedmask) != 0 )
            wt++;
    if ( wt > (numnotaries >> 1) || (wt > 7 && (signedmask & 1) != 0) )
        return 1;
    return 0;
}

/*****
 * Push keys into the notary collection
 * @param origheight the height where these notaries begin
 * @param pubkeys the notaries' public keys
 * @param num the number of keys in pubkeys
 */
void komodo_notarysinit(int32_t origheight,uint8_t pubkeys[64][33],int32_t num)
{
    if ( Pubkeys == 0 )
        Pubkeys = (knotaries_entry *)calloc(1 + (KOMODO_MAXBLOCKS / KOMODO_ELECTION_GAP),sizeof(*Pubkeys));
    knotaries_entry N;
    memset(&N,0,sizeof(N));
    // calculate the height
    int32_t htind = 0; // height index (number of elections so far)
    if ( origheight > 0 )
    {
        int32_t height = (origheight + KOMODO_ELECTION_GAP/2);
        height /= KOMODO_ELECTION_GAP;
        height = ((height + 1) * KOMODO_ELECTION_GAP);
        htind = ht_index_from_height(height);
    }
    {
        std::lock_guard<std::mutex> lock(komodo_mutex);
        for (int32_t k=0; k<num; k++)
        {
            knotary_entry *kp = (knotary_entry *)calloc(1,sizeof(*kp));
            memcpy(kp->pubkey,pubkeys[k],33);
            kp->notaryid = k;
            HASH_ADD_KEYPTR(hh,N.Notaries,kp->pubkey,33,kp);
        }
        N.numnotaries = num;
        for (int32_t i=htind; i<KOMODO_MAXBLOCKS / KOMODO_ELECTION_GAP; i++)
        {
            if ( Pubkeys[i].height != 0 && origheight < hwmheight )
            {
                printf("Pubkeys[%d].height %d < %d hwmheight, origheight.%d\n",i,Pubkeys[i].height,hwmheight,origheight);
                break;
            }
            Pubkeys[i] = N;
            Pubkeys[i].height = i * KOMODO_ELECTION_GAP;
        }
    }
    if ( origheight > hwmheight )
        hwmheight = origheight;
}

int32_t komodo_chosennotary(int32_t *notaryidp,int32_t height,uint8_t *pubkey33,uint32_t timestamp)
{
    // -1 if not notary, 0 if notary, 1 if special notary
    knotary_entry *kp; 
    int32_t numnotaries=0;
    int32_t modval = -1;

    *notaryidp = -1;
    if ( height < 0 )//|| height >= KOMODO_MAXBLOCKS )
    {
        printf("komodo_chosennotary ht.%d illegal\n",height);
        return(-1);
    }
    if ( height >= KOMODO_NOTARIES_HARDCODED || ASSETCHAINS_SYMBOL[0] != 0 )
    {
        if ( (*notaryidp= komodo_electednotary(&numnotaries,pubkey33,height,timestamp)) >= 0 && numnotaries != 0 )
        {
            modval = ((height % numnotaries) == *notaryidp);
            return(modval);
        }
    }
    if ( height >= 250000 )
        return(-1);
    if ( Pubkeys == nullptr )
        komodo_init(0);
    int32_t htind = ht_index_from_height(height);
    {
        std::lock_guard<std::mutex> lock(komodo_mutex);
        HASH_FIND(hh,Pubkeys[htind].Notaries,pubkey33,33,kp);
    }
    if ( kp != nullptr )
    {
        if ( (numnotaries= Pubkeys[htind].numnotaries) > 0 )
        {
            *notaryidp = kp->notaryid;
            modval = ((height % numnotaries) == kp->notaryid);
        } 
        else 
            printf("unexpected zero notaries at height.%d\n",height);
    }
    
    return modval;
}

/******
 * @brief Search the notarized checkpoints for a particular height
 * @note Finding a mach does include other criteria other than height
 *      such that the checkpoint includes the desired hight
 * @param height the key
 * @returns the checkpoint or nullptr
 */
const notarized_checkpoint *komodo_npptr(int32_t height)
{
    char symbol[KOMODO_ASSETCHAIN_MAXLEN];
    char dest[KOMODO_ASSETCHAIN_MAXLEN]; 

    komodo_state *sp = komodo_stateptr(symbol, dest);
    if ( sp != nullptr )
    {
        return sp->CheckpointAtHeight(height);
    }
    return nullptr;
}

/****
 * Search for the last (chronological) MoM notarized height
 * @returns the last notarized height that has a MoM
 */
int32_t komodo_prevMoMheight()
{
    char symbol[KOMODO_ASSETCHAIN_MAXLEN];
    char dest[KOMODO_ASSETCHAIN_MAXLEN];

    komodo_state *sp = komodo_stateptr(symbol,dest);
    if ( sp != nullptr )
    {
        return sp->PrevMoMHeight();
    }
    return 0;
}

/******
 * @brief Get the last notarization information
 * @param[out] prevMoMheightp the MoM height
 * @param[out] hashp the notarized hash
 * @param[out] txidp the DESTTXID
 * @returns the notarized height
 */
int32_t komodo_notarized_height(int32_t *prevMoMheightp,uint256 *hashp,uint256 *txidp)
{
    char symbol[KOMODO_ASSETCHAIN_MAXLEN];
    char dest[KOMODO_ASSETCHAIN_MAXLEN];

    *prevMoMheightp = 0;
    memset(hashp,0,sizeof(*hashp));
    memset(txidp,0,sizeof(*txidp));

    komodo_state *sp = komodo_stateptr(symbol, dest);
    if ( sp != nullptr )
    {
        return sp->NotarizedHeight(prevMoMheightp, hashp, txidp);
    }
    return 0;
}

int32_t komodo_dpowconfs(int32_t txheight,int32_t numconfs)
{
    static int32_t hadnotarization;
    char symbol[KOMODO_ASSETCHAIN_MAXLEN];
    char dest[KOMODO_ASSETCHAIN_MAXLEN];
    komodo_state *sp;

    if ( KOMODO_DPOWCONFS != 0 && txheight > 0 && numconfs > 0 && (sp= komodo_stateptr(symbol,dest)) != nullptr )
    {
        if ( sp->LastNotarizedHeight() > 0 )
        {
            hadnotarization = 1;
            if ( txheight < sp->LastNotarizedHeight() )
                return(numconfs);
            else return(1);
        }
        else if ( hadnotarization != 0 )
            return(1);
    }
    return(numconfs);
}

int32_t komodo_MoMdata(int32_t *notarized_htp,uint256 *MoMp,uint256 *kmdtxidp,int32_t height,uint256 *MoMoMp,int32_t *MoMoMoffsetp,int32_t *MoMoMdepthp,int32_t *kmdstartip,int32_t *kmdendip)
{
    const notarized_checkpoint *np = komodo_npptr(height);
    if ( np != nullptr )
    {
        *notarized_htp = np->notarized_height;
        *MoMp = np->MoM;
        *kmdtxidp = np->notarized_desttxid;
        *MoMoMp = np->MoMoM;
        *MoMoMoffsetp = np->MoMoMoffset;
        *MoMoMdepthp = np->MoMoMdepth;
        *kmdstartip = np->kmdstarti;
        *kmdendip = np->kmdendi;
        return(np->MoMdepth & 0xffff);
    }
    *notarized_htp = *MoMoMoffsetp = *MoMoMdepthp = *kmdstartip = *kmdendip = 0;
    memset(MoMp,0,sizeof(*MoMp));
    memset(MoMoMp,0,sizeof(*MoMoMp));
    memset(kmdtxidp,0,sizeof(*kmdtxidp));
    return 0;
}

/****
 * Get the notarization data below a particular height
 * @param[in] nHeight the height desired
 * @param[out] notarized_hashp the hash of the notarized block
 * @param[out] notarized_desttxidp the desttxid
 * @returns the notarized height
 */
int32_t komodo_notarizeddata(int32_t nHeight,uint256 *notarized_hashp,uint256 *notarized_desttxidp)
{
    char symbol[KOMODO_ASSETCHAIN_MAXLEN];
    char dest[KOMODO_ASSETCHAIN_MAXLEN];

    komodo_state *sp = komodo_stateptr(symbol,dest);
    if ( sp != nullptr )
    {
        return sp->NotarizedData(nHeight, notarized_hashp, notarized_desttxidp);
    }
    
    memset(notarized_hashp,0,sizeof(*notarized_hashp));
    memset(notarized_desttxidp,0,sizeof(*notarized_desttxidp));
    return 0;
}

/***
 * Add a notarized checkpoint to the komodo_state
 * @param[in] sp the komodo_state to add to
 * @param[in] nHeight the height
 * @param[in] notarized_height the height of the notarization
 * @param[in] notarized_hash the hash of the notarization
 * @param[in] notarized_desttxid the txid of the notarization on the destination chain
 * @param[in] MoM the MoM
 * @param[in] MoMdepth the depth
 */
void komodo_notarized_update(struct komodo_state *sp,int32_t nHeight,int32_t notarized_height,
        uint256 notarized_hash,uint256 notarized_desttxid,uint256 MoM,int32_t MoMdepth)
{
    if ( notarized_height >= nHeight )
    {
        fprintf(stderr,"komodo_notarized_update REJECT notarized_height %d > %d nHeight\n",notarized_height,nHeight);
        return;
    }

    notarized_checkpoint new_cp;
    new_cp.nHeight = nHeight;
    new_cp.notarized_height = notarized_height;
    new_cp.notarized_hash = notarized_hash;
    new_cp.notarized_desttxid = notarized_desttxid;
    new_cp.MoM = MoM;
    new_cp.MoMdepth = MoMdepth;
    std::lock_guard<std::mutex> lock(komodo_mutex);
    sp->AddCheckpoint(new_cp);
}

/****
 * @brief Initialize genesis notaries into memory
 * @note After a successful run, subsequent calls do nothing
 * @param height the current height (not used other than to stop initialization if less than zero)
 */
void komodo_init(int32_t height)
{
    uint256 zero; 
    int32_t k,n; 
    uint8_t pubkeys[64][33];
    memset(&zero,0,sizeof(zero));
    if ( !didinit )
    {
        decode_hex(NOTARY_PUBKEY33,33,NOTARY_PUBKEY.c_str());
        if ( height >= 0 )
        {
            int32_t k;
            uint8_t pubkeys[64][33];
            for (k=0; k<sizeof(Notaries_genesis)/sizeof(*Notaries_genesis); k++)
            {
                if ( Notaries_genesis[k][0] == 0 || Notaries_genesis[k][1] == 0 || Notaries_genesis[k][0][0] == 0 || Notaries_genesis[k][1][0] == 0 )
                    break;
                decode_hex(pubkeys[k],33,(char *)Notaries_genesis[k][1]);
            }
            komodo_notarysinit(0,pubkeys,k);
        }
        didinit = true;
        komodo_stateupdate(0,0,0,0,zero,0,0,0,0,0,0,0,0,0,0,zero,0);
    }
}

/****
 * A nasty hack to reset statics in this file.
 * Do not put this in komodo_notary.h, as it should only be used for testing
 */
void komodo_notaries_uninit()
{
    didinit = 0;
    hwmheight = 0;
    hadnotarization = 0;
    memset(&didinit_NOTARIES[0], 0, sizeof(uint8_t) * NUM_KMD_SEASONS);
    memset(&kmd_pubkeys[0], 0, sizeof(uint8_t) * NUM_KMD_SEASONS * 64 * 33);
    if (Pubkeys != nullptr)
    {
        // extern knotaries_entry *Pubkeys;
        /*
        knotaries_entry *key, *tmp;
        HASH_ITER(hh, Pubkeys, key, *tmp)
        {
            HASH_DEL(Pubkeys, key);
            free(key);
        }
        */
        free(Pubkeys);
        Pubkeys = nullptr;
    }
}
