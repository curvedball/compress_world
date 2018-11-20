


#include "zstd_compress_internal.h"
#include "zstd_double_fast.h"



void ZSTD_fillDoubleHashTable(ZSTD_matchState_t* ms, ZSTD_compressionParameters const* cParams, void const* end, ZSTD_dictTableLoadMethod_e dtlm)
{
    U32* const hashLarge = ms->hashTable;
    U32  const hBitsL = cParams->hashLog;
    U32  const mls = cParams->searchLength;
    U32* const hashSmall = ms->chainTable;
    U32  const hBitsS = cParams->chainLog;
    const BYTE* const base = ms->window.base;
    const BYTE* ip = base + ms->nextToUpdate;
    const BYTE* const iend = ((const BYTE*)end) - HASH_READ_SIZE;
    const U32 fastHashFillStep = 3;

    /* Always insert every fastHashFillStep position into the hash tables.
     * Insert the other positions into the large hash table if their entry
     * is empty.
     */
    for (; ip + fastHashFillStep - 1 <= iend; ip += fastHashFillStep)
    {
        U32 const current = (U32)(ip - base);
        U32 i;
        for (i = 0; i < fastHashFillStep; ++i)
        {
            size_t const smHash = ZSTD_hashPtr(ip + i, hBitsS, mls);
            size_t const lgHash = ZSTD_hashPtr(ip + i, hBitsL, 8);
            if (i == 0)
            {
                hashSmall[smHash] = current + i;
            }
            if (i == 0 || hashLarge[lgHash] == 0)
            {
                hashLarge[lgHash] = current + i;
            }
            /* Only load extra positions for ZSTD_dtlm_full */
            if (dtlm == ZSTD_dtlm_fast)
            {
                break;
            }
        }
    }
}



/*
======================================================
zb----LZ77-----algorithm:
	(L, M, D) denotes a matching item in LZ of LZFSE! //zbzbzbzb
	L=the number of Literals which are put into the literal buffer in compressing.
	M=Match Length
	D=Distance of the two matching iterms. In zstd code, D is called "offset"!
	
	The output of LZ algorithm is the literal buffer and a lot of (L, M, D) cells.		
======================================================	
*/
FORCE_INLINE_TEMPLATE size_t ZSTD_compressBlock_doubleFast_generic(ZSTD_matchState_t* ms, seqStore_t* seqStore, U32 rep[ZSTD_REP_NUM], ZSTD_compressionParameters const* cParams, void const* src, size_t srcSize, U32 const mls /* template */, ZSTD_dictMode_e const dictMode)
{
    U32* const hashLong = ms->hashTable;
    const U32 hBitsL = cParams->hashLog;
    U32* const hashSmall = ms->chainTable;
    const U32 hBitsS = cParams->chainLog;

	//
    const BYTE* const base = ms->window.base;
    const BYTE* const istart = (const BYTE*)src;
    const BYTE* ip = istart;
    const BYTE* anchor = istart;

	//
    const U32 prefixLowestIndex = ms->window.dictLimit;  //zb: value=0
    const BYTE* const prefixLowest = base + prefixLowestIndex;

	//
    const BYTE* const iend = istart + srcSize;
    const BYTE* const ilimit = iend - HASH_READ_SIZE;

	//
    U32 offset_1=rep[0], offset_2=rep[1]; //zb: vavlue1=1, value2=4
    U32 offsetSaved = 0;

	//
    const ZSTD_matchState_t* const dms = ms->dictMatchState;



	//
    const U32* const dictHashLong  = dictMode == ZSTD_dictMatchState ? dms->hashTable : NULL;
    const U32* const dictHashSmall = dictMode == ZSTD_dictMatchState ? dms->chainTable : NULL;
    const U32 dictStartIndex       = dictMode == ZSTD_dictMatchState ? dms->window.dictLimit : 0;
    const BYTE* const dictBase     = dictMode == ZSTD_dictMatchState ? dms->window.base : NULL;
    const BYTE* const dictStart    = dictMode == ZSTD_dictMatchState ? dictBase + dictStartIndex : NULL;
    const BYTE* const dictEnd      = dictMode == ZSTD_dictMatchState ? dms->window.nextSrc : NULL;
    const U32 dictIndexDelta       = dictMode == ZSTD_dictMatchState ? prefixLowestIndex - (U32)(dictEnd - dictBase) : 0;
    const U32 dictAndPrefixLength  = (U32)(ip - prefixLowest + dictEnd - dictStart);

    assert(dictMode == ZSTD_noDict || dictMode == ZSTD_dictMatchState);


#if 1
		DbgPrint("\n\n\n\n\n\n\nzb=================================ZSTD_compressBlock_doubleFast_generic===0.1======================\n");
		DbgPrint("hBitsL: %d\n", hBitsL);
		DbgPrint("hBitsS: %d\n", hBitsS);		
		DbgPrint("mls: %d\n\n", mls);			
		DbgPrint("dictMode: %d\n", dictMode);
		DbgPrint("dictHashLong: 0x%p\n", dictHashLong);
		DbgPrint("dictHashSmall: 0x%p\n", dictHashSmall); 
		DbgPrint("dictStartIndex: %d\n", dictStartIndex); 
		DbgPrint("dictIndexDelta: %d\n", dictIndexDelta);
		DbgPrint("dictAndPrefixLength: %d\n", dictAndPrefixLength);

		DbgPrint("istart: %p\n", istart);	
		DbgPrint("base: %p\n", base);	
		DbgPrint("ip: %p\n", ip);	
		DbgPrint("prefixLowestIndex: %d\n", prefixLowestIndex);	
		DbgPrint("prefixLowest: %p\n\n", prefixLowest);	
		DbgPrint("iend: %lp\n", iend);	
		DbgPrint("ilimit: %lp\n", ilimit);	
		DbgPrint("rep[0]: %d rep[1]: %d rep[2]: %d\n", rep[0], rep[1], rep[2]);
		DbgPrint("offset_1: %d offset_2: %d\n", offset_1, offset_2);
		DbgPrint("delta_Value: %d\n", (U32)(ip - prefixLowest));	
		DbgPrint("iend-istart: %d\n", (U32)(iend - istart));	
		
#endif

	//
	DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.0===\n");

    /* init */
    ip += (dictAndPrefixLength == 0); //zb: changed the value of the pointer ip!!!

	//
    if (dictMode == ZSTD_noDict)
    {
        U32 const maxRep = (U32)(ip - prefixLowest);

		//
		//DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.1===ip: %p prefixLowest: %p maxRep: %d\n", ip, prefixLowest, maxRep);

		//
        if (offset_2 > maxRep)
        {
			//DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.1a===offset_2: %d maxRep: %d\n", offset_2, maxRep);

			//zb:
            offsetSaved = offset_2, offset_2 = 0;
        }
		
        if (offset_1 > maxRep)
        {
			//DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.1b===offset_1: %d maxRep: %d\n", offset_1, maxRep);
            offsetSaved = offset_1, offset_1 = 0;
        }
    }
	
    if (dictMode == ZSTD_dictMatchState)
    {
		DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.2===\n\n\n");
        /* dictMatchState repCode checks don't currently handle repCode == 0
         * disabling. */
        assert(offset_1 <= dictAndPrefixLength);
        assert(offset_2 <= dictAndPrefixLength);
    }


	//====================================================================================
    while (ip < ilimit)     /*Main Search Loop of LZ algorithm,  instead of <=, because repcode check at (ip+1)  zbzbzbzbzb */
    {
		//DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.3====offset_1: %d offset_2: %d====\n", offset_1, offset_2);
		DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.3========\n"); //zb: offset_1=1, offset_2=0

		//
        size_t mLength;
        U32 offset;
        size_t const h2 = ZSTD_hashPtr(ip, hBitsL, 8);  //zb: Read U32 from the src_buffer and calculate its hash value!The second hash function: for 8-byte cell
        size_t const h = ZSTD_hashPtr(ip, hBitsS, mls); //zb:  Read U32 from the src_buffer and calculate its hash value!The first hash function: for 4-byte cell

		//
        U32 const current = (U32)(ip - base);

		//
        U32 const matchIndexL = hashLong[h2]; //zb: get the start position of the 8-byte sequence in the corresponding hash array! 
        U32 matchIndexS = hashSmall[h];		  //zb: get the start position of the 4-byte sequence in the corresponding hash array! 

		//
        const BYTE* matchLong = base + matchIndexL;
        const BYTE* match = base + matchIndexS;

		//
        const U32 repIndex = current + 1 - offset_1;
        const BYTE* repMatch = (dictMode == ZSTD_dictMatchState && repIndex < prefixLowestIndex) ? dictBase + (repIndex - dictIndexDelta) : base + repIndex;

		//
		//zb: put the start position of the 8-byte sequence in the corresponding hash array! 
		//
        hashLong[h2] = hashSmall[h] = current;   /* update hash tables */


        /* check dictMatchState repcode */
        if (dictMode == ZSTD_dictMatchState && ((U32)((prefixLowestIndex-1) - repIndex) >= 3 /* intentional underflow */) && (MEM_read32(repMatch) == MEM_read32(ip+1)) )
        {
			DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.3a===\n");
            const BYTE* repMatchEnd = repIndex < prefixLowestIndex ? dictEnd : iend;
            mLength = ZSTD_count_2segments(ip+1+4, repMatch+4, iend, repMatchEnd, prefixLowest) + 4;
            ip++;
            ZSTD_storeSeq(seqStore, ip-anchor, anchor, 0, mLength-MINMATCH);
            goto _match_stored;
        }

        /* check noDict repcode */
        if ( dictMode == ZSTD_noDict && ((offset_1 > 0) & (MEM_read32(ip + 1 - offset_1) == MEM_read32(ip + 1))))
        {
			DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.3b===\n");
            mLength = ZSTD_count(ip+1+4, ip+1+4-offset_1, iend) + 4;
            ip++;
            ZSTD_storeSeq(seqStore, ip-anchor, anchor, 0, mLength-MINMATCH);
            goto _match_stored;
        }

		//====================================================
        /* check prefix long match */
        if ( (matchIndexL > prefixLowestIndex) && (MEM_read64(matchLong) == MEM_read64(ip)) )
        {
			DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.3c===\n");
            mLength = ZSTD_count(ip+8, matchLong+8, iend) + 8;
            offset = (U32)(ip-matchLong);
            while (((ip>anchor) & (matchLong>prefixLowest)) && (ip[-1] == matchLong[-1]))
            {
                ip--;    /* catch up */
                matchLong--;
                mLength++;
            }
            goto _match_found;
        }

        /* check dictMatchState long match */
        if (dictMode == ZSTD_dictMatchState)
        {
			DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.3d===\n");
            U32 const dictMatchIndexL = dictHashLong[h2];
            const BYTE* dictMatchL = dictBase + dictMatchIndexL;
            assert(dictMatchL < dictEnd);

            if (dictMatchL > dictStart && MEM_read64(dictMatchL) == MEM_read64(ip))
            {
                mLength = ZSTD_count_2segments(ip+8, dictMatchL+8, iend, dictEnd, prefixLowest) + 8;
                offset = (U32)(current - dictMatchIndexL - dictIndexDelta);
                while (((ip>anchor) & (dictMatchL>dictStart)) && (ip[-1] == dictMatchL[-1]))
                {
                    ip--;    /* catch up */
                    dictMatchL--;
                    mLength++;
                }
                goto _match_found;
            }
        }

		//====================================================
        /* check prefix short match */
        if ((matchIndexS > prefixLowestIndex) && (MEM_read32(match) == MEM_read32(ip)))
        {
			DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.3e===matchIndexS: %d prefixLowestIndex: %d\n", matchIndexS, prefixLowestIndex);
            goto _search_next_long;
        }

        /* check dictMatchState short match */
        if (dictMode == ZSTD_dictMatchState)
        {
			DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.3f===\n");
            U32 const dictMatchIndexS = dictHashSmall[h];
            match = dictBase + dictMatchIndexS;
            matchIndexS = dictMatchIndexS + dictIndexDelta;

            if (match > dictStart && MEM_read32(match) == MEM_read32(ip))
            {
                goto _search_next_long;
            }
        }
		
		//
		//zbzb: loop frequently!!!
		//
		//DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.4===\n");
        ip += ((ip - anchor) >> kSearchStrength) + 1; //zb: search jump step=1(in sequence range 0~255), step=2(in sequence range 256~511), step=3(in sequence range 512~767)
        continue;	


	    //===========================================
    _search_next_long:
        DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.5===\n");
        {
            size_t const hl3 = ZSTD_hashPtr(ip + 1, hBitsL, 8);
            U32 const matchIndexL3 = hashLong[hl3];
            const BYTE* matchL3 = base + matchIndexL3;
            hashLong[hl3] = current + 1;

            /* check prefix long +1 match */
            if ( (matchIndexL3 > prefixLowestIndex) && (MEM_read64(matchL3) == MEM_read64(ip + 1)) )
            {
				DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.5a===\n");
                mLength = ZSTD_count(ip+9, matchL3+8, iend) + 8;
                ip++;
                offset = (U32)(ip-matchL3);
                while (((ip>anchor) & (matchL3>prefixLowest)) && (ip[-1] == matchL3[-1]))
                {
                    ip--;    /* catch up */
                    matchL3--;
                    mLength++;
                }
                goto _match_found;
            }

            /* check dict long +1 match */
            if (dictMode == ZSTD_dictMatchState)
            {
				DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.5b===\n");
                U32 const dictMatchIndexL3 = dictHashLong[hl3];
                const BYTE* dictMatchL3 = dictBase + dictMatchIndexL3;
                assert(dictMatchL3 < dictEnd);
                if (dictMatchL3 > dictStart && MEM_read64(dictMatchL3) == MEM_read64(ip+1))
                {
                    mLength = ZSTD_count_2segments(ip+1+8, dictMatchL3+8, iend, dictEnd, prefixLowest) + 8;
                    ip++;
                    offset = (U32)(current + 1 - dictMatchIndexL3 - dictIndexDelta);
                    while (((ip>anchor) & (dictMatchL3>dictStart)) && (ip[-1] == dictMatchL3[-1]))
                    {
                        ip--;    /* catch up */
                        dictMatchL3--;
                        mLength++;
                    }
                    goto _match_found;
                }
            }
        }

        DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.6===\n");

        /* if no long +1 match, explore the short match we found */
        if (dictMode == ZSTD_dictMatchState && matchIndexS < prefixLowestIndex)
        {
			DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.6a===\n");
            mLength = ZSTD_count_2segments(ip+4, match+4, iend, dictEnd, prefixLowest) + 4;
            offset = (U32)(current - matchIndexS);
            while (((ip>anchor) & (match>dictStart)) && (ip[-1] == match[-1]))
            {
                ip--;    /* catch up */
                match--;
                mLength++;
            }
        }
        else
        {
			DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.6b===\n");

			//
            mLength = ZSTD_count(ip + 4, match + 4, iend) + 4; //zb: Search forward (can return the length of the common prefix include the 4-bytes_or_8-bytes)!

			//
            offset = (U32)(ip - match);

			//
            while (((ip > anchor) & (match > prefixLowest)) && (ip[-1] == match[-1])) //zb: Search backward
            {
                ip--;    /* catch up */
                match--;
                mLength++;
            }
        }

        /* fall-through */
    _match_found:
    	DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===_match_found===offset_1: %d offset: %d mLength: %d\n", offset_1, offset, mLength);

		//
        offset_2 = offset_1;
        offset_1 = offset;

		//
        ZSTD_storeSeq(seqStore, ip - anchor, anchor, offset + ZSTD_REP_MOVE, mLength - MINMATCH);

    _match_stored:
        /* match found */
        ip += mLength;
        anchor = ip;

		DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.7===\n");

		//
        if (ip <= ilimit)
        {
			DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.8===\n");

			//
            /* Fill Table */
            hashLong[ZSTD_hashPtr(base + current + 2, hBitsL, 8)] = hashSmall[ZSTD_hashPtr(base + current + 2, hBitsS, mls)] = current + 2;  /* here because current+2 could be > iend-8 */
            hashLong[ZSTD_hashPtr(ip - 2, hBitsL, 8)] = hashSmall[ZSTD_hashPtr(ip - 2, hBitsS, mls)] = (U32)(ip - 2 - base);

            /* check immediate repcode */
            if (dictMode == ZSTD_dictMatchState)
            {
				DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.8a===\n");
                while (ip <= ilimit)
                {
                    U32 const current2 = (U32)(ip-base);
                    U32 const repIndex2 = current2 - offset_2;
                    const BYTE* repMatch2 = dictMode == ZSTD_dictMatchState && repIndex2 < prefixLowestIndex ? dictBase - dictIndexDelta + repIndex2 : base + repIndex2;
                    if ( ((U32)((prefixLowestIndex-1) - (U32)repIndex2) >= 3 /* intentional overflow */) && (MEM_read32(repMatch2) == MEM_read32(ip)) )
                    {
                        const BYTE* const repEnd2 = repIndex2 < prefixLowestIndex ? dictEnd : iend;
                        size_t const repLength2 = ZSTD_count_2segments(ip+4, repMatch2+4, iend, repEnd2, prefixLowest) + 4;
                        U32 tmpOffset = offset_2;
                        offset_2 = offset_1;
                        offset_1 = tmpOffset;   /* swap offset_2 <=> offset_1 */
                        ZSTD_storeSeq(seqStore, 0, anchor, 0, repLength2-MINMATCH);
                        hashSmall[ZSTD_hashPtr(ip, hBitsS, mls)] = current2;
                        hashLong[ZSTD_hashPtr(ip, hBitsL, 8)] = current2;
                        ip += repLength2;
                        anchor = ip;
                        continue;
                    }
                    break;
                }
            }

            if (dictMode == ZSTD_noDict)
            {
				DbgPrint("zb===ZSTD_compressBlock_doubleFast_generic===1.8b===\n");

				//
                while ( (ip <= ilimit) && ( (offset_2 > 0) & (MEM_read32(ip) == MEM_read32(ip - offset_2)) ))
                {
                    /* store sequence */
                    size_t const rLength = ZSTD_count(ip+4, ip+4-offset_2, iend) + 4;
                    U32 const tmpOff = offset_2;
                    offset_2 = offset_1;
                    offset_1 = tmpOff;  /* swap offset_2 <=> offset_1 */
                    hashSmall[ZSTD_hashPtr(ip, hBitsS, mls)] = (U32)(ip-base);
                    hashLong[ZSTD_hashPtr(ip, hBitsL, 8)] = (U32)(ip-base);
                    ZSTD_storeSeq(seqStore, 0, anchor, 0, rLength-MINMATCH);
                    ip += rLength;
                    anchor = ip;
                    continue;   /* faster when present ... (?) */
                }
            }
        }
    }

    /* save reps for next block */
    rep[0] = offset_1 ? offset_1 : offsetSaved;
    rep[1] = offset_2 ? offset_2 : offsetSaved;

    /* Return the last literals size */
    return iend - anchor;
}



size_t ZSTD_compressBlock_doubleFast(ZSTD_matchState_t* ms, seqStore_t* seqStore, U32 rep[ZSTD_REP_NUM], ZSTD_compressionParameters const* cParams, void const* src, size_t srcSize)
{
    const U32 mls = cParams->searchLength;
	DbgPrint("zb===ZSTD_compressBlock_doubleFast===1.0===mls: %d\n\n\n", mls);

	
    switch(mls)
    {
        default: /* includes case 3 */
        case 4 :
            return ZSTD_compressBlock_doubleFast_generic(ms, seqStore, rep, cParams, src, srcSize, 4, ZSTD_noDict); //zb:

		
        case 5 :
			//
            return ZSTD_compressBlock_doubleFast_generic(ms, seqStore, rep, cParams, src, srcSize, 5, ZSTD_noDict);
			
        case 6 :
            return ZSTD_compressBlock_doubleFast_generic(ms, seqStore, rep, cParams, src, srcSize, 6, ZSTD_noDict);
			
        case 7 :
            return ZSTD_compressBlock_doubleFast_generic(ms, seqStore, rep, cParams, src, srcSize, 7, ZSTD_noDict);
    }
}



static size_t ZSTD_compressBlock_doubleFast_extDict_generic(ZSTD_matchState_t* ms, seqStore_t* seqStore, U32 rep[ZSTD_REP_NUM], ZSTD_compressionParameters const* cParams, void const* src, size_t srcSize, U32 const mls /* template */)
{
    U32* const hashLong = ms->hashTable;
    U32  const hBitsL = cParams->hashLog;
    U32* const hashSmall = ms->chainTable;
    U32  const hBitsS = cParams->chainLog;
    const BYTE* const istart = (const BYTE*)src;
    const BYTE* ip = istart;
    const BYTE* anchor = istart;
    const BYTE* const iend = istart + srcSize;
    const BYTE* const ilimit = iend - 8;
    const U32   prefixStartIndex = ms->window.dictLimit;
    const BYTE* const base = ms->window.base;
    const BYTE* const prefixStart = base + prefixStartIndex;
    const U32   dictStartIndex = ms->window.lowLimit;
    const BYTE* const dictBase = ms->window.dictBase;
    const BYTE* const dictStart = dictBase + dictStartIndex;
    const BYTE* const dictEnd = dictBase + prefixStartIndex;
    U32 offset_1=rep[0], offset_2=rep[1];

    DEBUGLOG(5, "ZSTD_compressBlock_doubleFast_extDict_generic (srcSize=%zu)", srcSize);

    /* Search Loop */
    while (ip < ilimit)    /* < instead of <=, because (ip+1) */
    {
        const size_t hSmall = ZSTD_hashPtr(ip, hBitsS, mls);
        const U32 matchIndex = hashSmall[hSmall];
        const BYTE* const matchBase = matchIndex < prefixStartIndex ? dictBase : base;
        const BYTE* match = matchBase + matchIndex;

        const size_t hLong = ZSTD_hashPtr(ip, hBitsL, 8);
        const U32 matchLongIndex = hashLong[hLong];
        const BYTE* const matchLongBase = matchLongIndex < prefixStartIndex ? dictBase : base;
        const BYTE* matchLong = matchLongBase + matchLongIndex;

        const U32 current = (U32)(ip-base);
        const U32 repIndex = current + 1 - offset_1;   /* offset_1 expected <= current +1 */
        const BYTE* const repBase = repIndex < prefixStartIndex ? dictBase : base;
        const BYTE* const repMatch = repBase + repIndex;
        size_t mLength;
        hashSmall[hSmall] = hashLong[hLong] = current;   /* update hash table */

        if ((((U32)((prefixStartIndex-1) - repIndex) >= 3) /* intentional underflow : ensure repIndex doesn't overlap dict + prefix */
             & (repIndex > dictStartIndex))
            && (MEM_read32(repMatch) == MEM_read32(ip+1)) )
        {
            const BYTE* repMatchEnd = repIndex < prefixStartIndex ? dictEnd : iend;
            mLength = ZSTD_count_2segments(ip+1+4, repMatch+4, iend, repMatchEnd, prefixStart) + 4;
            ip++;
            ZSTD_storeSeq(seqStore, ip-anchor, anchor, 0, mLength-MINMATCH);
        }
        else
        {
            if ((matchLongIndex > dictStartIndex) && (MEM_read64(matchLong) == MEM_read64(ip)))
            {
                const BYTE* const matchEnd = matchLongIndex < prefixStartIndex ? dictEnd : iend;
                const BYTE* const lowMatchPtr = matchLongIndex < prefixStartIndex ? dictStart : prefixStart;
                U32 offset;
                mLength = ZSTD_count_2segments(ip+8, matchLong+8, iend, matchEnd, prefixStart) + 8;
                offset = current - matchLongIndex;
                while (((ip>anchor) & (matchLong>lowMatchPtr)) && (ip[-1] == matchLong[-1]))
                {
                    ip--;    /* catch up */
                    matchLong--;
                    mLength++;
                }
                offset_2 = offset_1;
                offset_1 = offset;
                ZSTD_storeSeq(seqStore, ip-anchor, anchor, offset + ZSTD_REP_MOVE, mLength-MINMATCH);

            }
            else if ((matchIndex > dictStartIndex) && (MEM_read32(match) == MEM_read32(ip)))
            {
                size_t const h3 = ZSTD_hashPtr(ip+1, hBitsL, 8);
                U32 const matchIndex3 = hashLong[h3];
                const BYTE* const match3Base = matchIndex3 < prefixStartIndex ? dictBase : base;
                const BYTE* match3 = match3Base + matchIndex3;
                U32 offset;
                hashLong[h3] = current + 1;
                if ( (matchIndex3 > dictStartIndex) && (MEM_read64(match3) == MEM_read64(ip+1)) )
                {
                    const BYTE* const matchEnd = matchIndex3 < prefixStartIndex ? dictEnd : iend;
                    const BYTE* const lowMatchPtr = matchIndex3 < prefixStartIndex ? dictStart : prefixStart;
                    mLength = ZSTD_count_2segments(ip+9, match3+8, iend, matchEnd, prefixStart) + 8;
                    ip++;
                    offset = current+1 - matchIndex3;
                    while (((ip>anchor) & (match3>lowMatchPtr)) && (ip[-1] == match3[-1]))
                    {
                        ip--;    /* catch up */
                        match3--;
                        mLength++;
                    }
                }
                else
                {
                    const BYTE* const matchEnd = matchIndex < prefixStartIndex ? dictEnd : iend;
                    const BYTE* const lowMatchPtr = matchIndex < prefixStartIndex ? dictStart : prefixStart;
                    mLength = ZSTD_count_2segments(ip+4, match+4, iend, matchEnd, prefixStart) + 4;
                    offset = current - matchIndex;
                    while (((ip>anchor) & (match>lowMatchPtr)) && (ip[-1] == match[-1]))
                    {
                        ip--;    /* catch up */
                        match--;
                        mLength++;
                    }
                }
                offset_2 = offset_1;
                offset_1 = offset;
                ZSTD_storeSeq(seqStore, ip-anchor, anchor, offset + ZSTD_REP_MOVE, mLength-MINMATCH);

            }
            else
            {
                ip += ((ip-anchor) >> kSearchStrength) + 1;
                continue;
            }
        }

        /* found a match : store it */
        ip += mLength;
        anchor = ip;

        if (ip <= ilimit)
        {
            /* Fill Table */
            hashSmall[ZSTD_hashPtr(base+current+2, hBitsS, mls)] = current+2;
            hashLong[ZSTD_hashPtr(base+current+2, hBitsL, 8)] = current+2;
            hashSmall[ZSTD_hashPtr(ip-2, hBitsS, mls)] = (U32)(ip-2-base);
            hashLong[ZSTD_hashPtr(ip-2, hBitsL, 8)] = (U32)(ip-2-base);
            /* check immediate repcode */
            while (ip <= ilimit)
            {
                U32 const current2 = (U32)(ip-base);
                U32 const repIndex2 = current2 - offset_2;
                const BYTE* repMatch2 = repIndex2 < prefixStartIndex ? dictBase + repIndex2 : base + repIndex2;
                if ( (((U32)((prefixStartIndex-1) - repIndex2) >= 3)   /* intentional overflow : ensure repIndex2 doesn't overlap dict + prefix */
                      & (repIndex2 > dictStartIndex))
                     && (MEM_read32(repMatch2) == MEM_read32(ip)) )
                {
                    const BYTE* const repEnd2 = repIndex2 < prefixStartIndex ? dictEnd : iend;
                    size_t const repLength2 = ZSTD_count_2segments(ip+4, repMatch2+4, iend, repEnd2, prefixStart) + 4;
                    U32 const tmpOffset = offset_2;
                    offset_2 = offset_1;
                    offset_1 = tmpOffset;   /* swap offset_2 <=> offset_1 */
                    ZSTD_storeSeq(seqStore, 0, anchor, 0, repLength2-MINMATCH);
                    hashSmall[ZSTD_hashPtr(ip, hBitsS, mls)] = current2;
                    hashLong[ZSTD_hashPtr(ip, hBitsL, 8)] = current2;
                    ip += repLength2;
                    anchor = ip;
                    continue;
                }
                break;
            }
        }
    }

    /* save reps for next block */
    rep[0] = offset_1;
    rep[1] = offset_2;

    /* Return the last literals size */
    return iend - anchor;
}




size_t ZSTD_compressBlock_doubleFast_extDict(ZSTD_matchState_t* ms, seqStore_t* seqStore, U32 rep[ZSTD_REP_NUM], ZSTD_compressionParameters const* cParams, void const* src, size_t srcSize)
{
    U32 const mls = cParams->searchLength;
    switch(mls)
    {
        default: /* includes case 3 */
        case 4 :
            return ZSTD_compressBlock_doubleFast_extDict_generic(ms, seqStore, rep, cParams, src, srcSize, 4);
        case 5 :
            return ZSTD_compressBlock_doubleFast_extDict_generic(ms, seqStore, rep, cParams, src, srcSize, 5);
        case 6 :
            return ZSTD_compressBlock_doubleFast_extDict_generic(ms, seqStore, rep, cParams, src, srcSize, 6);
        case 7 :
            return ZSTD_compressBlock_doubleFast_extDict_generic(ms, seqStore, rep, cParams, src, srcSize, 7);
    }
}





