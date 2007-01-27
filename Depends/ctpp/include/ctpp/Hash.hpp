/*
 * Copyright (c) 2006 FTS Team
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the FTS Team nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      Hash.hpp
 *
 * $FTS$
 */
#ifndef _HASH_HPP__
#define _HASH_HPP__ 1

/**
  @file Hash.hpp
  @brief Generic hash class
*/

#include "Types.h"

namespace CTPP
{

const UINT_64 C_USED_CELL = 0x8000000000000000ll;
const UINT_64 C_ERASED_CELL = 0x4000000000000000ll;
const UINT_64 C_MAX_HASH_MASK = 0x3FFFFFFFFFFFFFFFll;

/**
  @struct Pair Hash.hpp <Hash.hpp>
  @brief Pair
*/
template<class TKey, class TVal>struct Pair
{
	/** Key   */
	TKey     first;
	/** Value */
	TVal     second;

	/**
	  @brief Constructor
	  @param oKey - initial key
	  @param oVal - initial value
	*/
	inline Pair(const TKey & oKey = TKey(), const TVal & oVal = TVal()): first(oKey), second(oVal) { ;; }

	/**
	  @brief Copy Constructor
	  @param oPair - another pair
	*/
	inline Pair(const Pair<TKey, TVal> & oPair): first(oPair.first), second(oPair.second) { ;; }
};

/**
  @class Hash Hash.hpp <Hash.hpp>
  @brief Hash template
*/
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>class Hash
{
private:

	/** Hash table   */
	struct HashTable
	{
		/** Hash                */
		TokenHash_t        hash;
		/** Stored data         */
		Pair<TKey, TVal>   data;
		/** Default constructor */
		inline HashTable(): hash(0) { ;; }
	};

	/** Hash storage */
	struct HashStorage
	{
		/** Maximum hash size      */
		INT_32                  max_size;
		/** Used elements          */
		INT_32                  used;
		/** Hash bitmask           */
		INT_32                  bit_mask;
		/** hash table             */
		HashTable             * hash_table;
	};

public:
	/**
	  @brief Constructor
	  @param iIMaxSize - maximum hash size
	*/
	Hash(INT_32 iIMaxSize);

	/**
	  @brief Copy Constructor
	  @param oHashIdx - another object
	*/
	Hash(const Hash & oHashIdx);

	/**
	  @brief Copy operator
	  @param oHashIdx - another object
	*/
	Hash & operator=(const Hash & oHashIdx);

	/**
	  @brief A Destructor
	*/
	~Hash() throw();

	/**
	  @brief Put element into hash
	  @param oPair - pair of key => value
	*/
	INT_32 insert(const Pair<TKey, TVal> & oPair);

	/**
	  @brief Put element into hash
	  @param oPair - pair of key => value
	  @param iHash - key hash
	*/
	INT_32 insert(const Pair<TKey, TVal> & oPair, TokenHash_t iHash);

	/**
	  @brief Clear all elements from hash
	*/
	void clear();

	/**
	  @brief Remove element from hash
	  @param oKey - element key
	*/
	void erase(const TKey & oKey);

	/**
	  @brief Remove element from hash
	  @param oKey - element key
	  @param iHash - key hash
	*/
	void erase(const TKey & oKey, TokenHash_t iHash);

	/**
	  @brief Get actual hash size
	  @return actual hash size
	*/
	inline INT_32 size() { return pStorage -> used; }

	/**
	  @brief Get maximum hash size
	  @return maximum hash size
	*/
	inline INT_32 capacity() { return pStorage -> max_size; }

	/**
	  @class base_iterator Hash.hpp <Hash.hpp>
	  @brief Hash iterator
	*/
	template <typename TKeyIt, typename TValIt, typename THashFNIt, typename TCmpFNIt>class base_iterator
	{
	private:
		/** Hash storage              */
		HashStorage           * pItStorage;
		/** Current iterator position */
		INT_32                  iPos;

		public:

		/**
		  @brief Prefix operator ++
		*/
		inline base_iterator(): pItStorage(NULL), iPos(0) { ;; }

		/**
		  @brief Constructor
		*/
		inline  base_iterator(HashStorage * pIStorage, INT_32 iIPos): pItStorage(pIStorage), iPos(iIPos)
		{
			while(iPos != pItStorage -> max_size && !(pItStorage -> hash_table[iPos].hash & C_USED_CELL)) { iPos++; }
		}

		/**
		  @brief Pointer operator ->
		*/
		inline Pair<TKeyIt, TValIt> * operator->() { return &(pItStorage -> hash_table[iPos].data); }

		/**
		  @brief Reference operator *
		*/
		inline Pair<TKeyIt, TValIt> & operator*() { return pItStorage -> hash_table[iPos].data; }

		/**
		  @brief Copy operator =
		*/
		inline base_iterator<TKeyIt, TValIt, THashFNIt, TCmpFNIt> & operator=(const base_iterator<TKeyIt, TValIt, THashFNIt, TCmpFNIt> & oRhs)
		{
			pItStorage = oRhs.pItStorage;
			iPos       = oRhs.iPos;
			return *this;
		}

		/**
		  @brief operator ==
		  @return true if iterators are equal
		*/
		inline bool operator==(const base_iterator<TKeyIt, TValIt, THashFNIt, TCmpFNIt> & oRhs) { return ((pItStorage == oRhs.pItStorage) && (iPos == oRhs.iPos)); }

		/**
		  @brief operator !=
		  @return true if iterators are NOT equal
		*/
		inline bool operator!=(const base_iterator<TKeyIt, TValIt, THashFNIt, TCmpFNIt> & oRhs) { return (!((pItStorage == oRhs.pItStorage) && (iPos == oRhs.iPos))); }

		/**
		  @brief Prefix operator ++
		*/
		inline base_iterator<TKeyIt, TValIt, THashFNIt, TCmpFNIt> operator++()
		{
			if (iPos == pItStorage -> max_size) { return *this; }
			iPos++;
			while(1)
			{
				if (iPos == pItStorage -> max_size) { break; }
				if (pItStorage -> hash_table[iPos].hash & C_USED_CELL) { break; }
				iPos++;
			}
			return *this;
		}

		/**
		  @brief Postfix operator ++
		*/
		base_iterator<TKeyIt, TValIt, THashFNIt, TCmpFNIt> operator++(int)
		{
			if (iPos == pItStorage -> max_size) { return *this; }
			base_iterator<TKeyIt, TValIt, THashFNIt, TCmpFNIt> oTMP(*this);
			iPos++;
			while(1)
			{
				if (iPos == pItStorage -> max_size) { break; }
				if (pItStorage -> hash_table[iPos].hash & C_USED_CELL) { break; }
				iPos++;
			}
			return oTMP;
		}
	};


	/** Normal iterator   */
	typedef base_iterator<TKey, TVal, THashFN, TCmpFN>  iterator;
	/** Constant iterator */
	typedef base_iterator<const TKey, const TVal, const THashFN, const TCmpFN> const_iterator;

	/** First element          */
	inline iterator begin()             { return iterator(pStorage, 0);       }
	/** First constant element */
	inline const_iterator begin() const { return const_iterator(pStorage, 0); }

	/** Last element           */
	inline iterator end()               { return iterator(pStorage, pStorage -> max_size);       }
	/** Last constant element  */
	inline const_iterator end() const   { return const_iterator(pStorage, pStorage -> max_size); }

	/**
	  @brief Search element in hash
	  @param oKey - key to find
	  @return itarator to the element or end() if nothing found
	*/
	inline iterator find(const TKey & oKey)
	{
		INT_32 iOffset = 0;
		if (FindElement(oKey, iOffset) == 0) { return end(); }
		return iterator(pStorage, iOffset);
	}

	/**
	  @brief Search element in hash
	  @param oKey - key to find
	  @return itarator to the element or end() if nothing found
	*/
	inline const_iterator find(const TKey & oKey) const
	{
		INT_32 iOffset = 0;
		if (FindElement(oKey, iOffset) == 0) { return end(); }
		return const_iterator(pStorage, iOffset);
	}

	/**
	  @brief Search element in hash
	  @param oKey - key to find
	  @param iHash - key hash
	  @return itarator to the element or end() if nothing found
	*/
	inline iterator find(const TKey & oKey, const TokenHash_t & iHash)
	{
		INT_32 iOffset = 0;
		if (FindElement(oKey, iHash, iOffset) == 0) { return end(); }
		return iterator(pStorage, iOffset);
	}

	/**
	  @brief Search element in hash
	  @param oKey - key to find
	  @param iHash - key hash
	  @return itarator to the element or end() if nothing found
	*/
	inline const_iterator find(const TKey & oKey, const TokenHash_t & iHash) const
	{
		INT_32 iOffset = 0;
		if (FindElement(oKey, iHash, iOffset) == 0) { return end(); }
		return const_iterator(pStorage, iOffset);
	}

private:
	/** Hasher                 */
	THashFN         oHashFun;
	/** Comparator             */
	TCmpFN          oCmpFun;

	/** Hash storage           */
	HashStorage   * pStorage;

	/**
	  @brief Find offset of element in hash
	  @param oKey - key to find
	  @param iOffset - offset of key in hash
	  @return 1 if found, 0 if - not
	*/
	INT_32 FindElement(const TKey & oKey, INT_32 &iOffset) const;

	/**
	  @brief Find offset of element in hash
	  @param oKey - key to find
	  @param iHash - hash of oKey
	  @param iOffset - offset of key in hash
	  @return 1 if found, 0 if - not
	*/
	INT_32 FindElement(const TKey & oKey, TokenHash_t iHash, INT_32 &iOffset) const;

	/**
	  @brief Expand hash
	*/
	void expand();
};

//
// Constructor
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
  Hash<TKey, TVal, THashFN, TCmpFN>::Hash(const INT_32 iIMaxSize)
{
	pStorage = new HashStorage;
	// Empty hash
	pStorage -> max_size = iIMaxSize;
	pStorage -> used     = 0;

	// Nearest to iIMaxSize power of 2
	INT_32 iCount = 0;
	while(pStorage -> max_size) { pStorage -> max_size  >>= 1; iCount++; }
	pStorage -> max_size  = 1 << iCount;
	if (pStorage -> max_size >> 1 == iIMaxSize) { pStorage -> max_size  = iIMaxSize; }
	pStorage -> bit_mask = pStorage -> max_size - 1;

	pStorage -> hash_table = new HashTable[pStorage -> max_size];
}

//
// Copy constructor
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
  Hash<TKey, TVal, THashFN, TCmpFN>::Hash(const Hash & oRhs)
{
	pStorage = new HashStorage;
	pStorage -> max_size   = oRhs.pStorage -> max_size;
	pStorage -> used       = oRhs.pStorage -> used;
	pStorage -> bit_mask   = oRhs.pStorage -> bit_mask;
	pStorage -> hash_table = new HashTable[pStorage -> max_size];

	// Copy Elements
	for (INT_32 iI = 0; iI <= pStorage -> max_size; iI++) { pStorage -> hash_table[iI] = oRhs.pStorage -> hash_table[iI]; }
}

//
// Operator =
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
  Hash<TKey, TVal, THashFN, TCmpFN> & Hash<TKey, TVal, THashFN, TCmpFN>::operator=(const Hash & oRhs)
{
	delete pStorage -> hash_table;

	pStorage -> max_size = oRhs.pStorage -> max_size;
	pStorage -> used     = oRhs.pStorage -> used;
	pStorage -> bit_mask = oRhs.pStorage -> bit_mask;

	pStorage -> hash_table = new HashTable[pStorage -> max_size + 1];

	// Copy Elements
	for (INT_32 iI = 0; iI < pStorage -> max_size; iI++) { pStorage -> hash_table[iI] = oRhs.pStorage -> hash_table[iI]; }

return *this;
}

//
// Destructor
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
  Hash<TKey, TVal, THashFN, TCmpFN>::~Hash() throw()
{
	delete [] pStorage -> hash_table;
	delete pStorage;
}

//
// Put element to hash
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
  INT_32 Hash<TKey, TVal, THashFN, TCmpFN>::insert(const Pair<TKey, TVal> & oPair, TokenHash_t iHash)
{
	// Expand hash if need
	if (3 * pStorage -> used / 2 >= pStorage -> max_size) { expand(); return insert(oPair, iHash); }

	// Hash
	iHash           = iHash & C_MAX_HASH_MASK;
	// Initial offset
	INT_32  iOffset = iHash & pStorage -> bit_mask;
	INT_32  iDuplicate = 0;
	// Find element
	if (iOffset == pStorage -> max_size) { expand(); return insert(oPair, iHash); }
	else
	{
		// While cell is used
		while(pStorage -> hash_table[iOffset].hash & C_USED_CELL)
		{
			// If same element
			if ((pStorage -> hash_table[iOffset].hash & C_MAX_HASH_MASK) == iHash)
			{
				if (oCmpFun(pStorage -> hash_table[iOffset].data.first, oPair.first)) { iDuplicate = 1; break; }
			}
			iOffset++;

			// End of hash
			if (iOffset == pStorage -> max_size) { expand(); return insert(oPair, iHash); }
		}
	}
	if (iDuplicate == 0) { ++(pStorage -> used); }

	pStorage -> hash_table[iOffset].hash = iHash | C_USED_CELL;
	pStorage -> hash_table[iOffset].data = oPair;
// Okay
return 0;
}

//
// Put element to hash
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
  INT_32 Hash<TKey, TVal, THashFN, TCmpFN>::insert(const Pair<TKey, TVal> & oPair)
{
	// Hash
	TokenHash_t iHash = oHashFun(oPair.first);

return insert(oPair, iHash);
}

//
// Clear all elements from hash
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
void Hash<TKey, TVal, THashFN, TCmpFN>::clear()
{
	Hash<TKey, TVal, THashFN, TCmpFN> oTMP(1);
	*this = oTMP;
}

//
// Remove element from hash
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
  void Hash<TKey, TVal, THashFN, TCmpFN>::erase(const TKey & oKey)
{
	if (pStorage -> used == 0) { return; }
	// Offset
	INT_32 iOffset;
	// Find element
	if (FindElement(oKey, iOffset) != 0)
	{
		--(pStorage -> used);
		// Destroy element
		pStorage -> hash_table[iOffset].hash = C_ERASED_CELL;
		pStorage -> hash_table[iOffset].data = Pair<TKey, TVal>();
	}
}

//
// Remove element from hash
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
  void Hash<TKey, TVal, THashFN, TCmpFN>::erase(const TKey & oKey, TokenHash_t iHash)
{
	if (pStorage -> used == 0) { return; }
	// Offset
	INT_32 iOffset;
	// Find element
	if (FindElement(oKey, iHash, iOffset) != 0)
	{
		--(pStorage -> used);
		// Destroy element
		pStorage -> hash_table[iOffset].hash = C_ERASED_CELL;
		pStorage -> hash_table[iOffset].data = Pair<TKey, TVal>();
	}
}

//
// Search element
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
  INT_32 Hash<TKey, TVal, THashFN, TCmpFN>::FindElement(const TKey & oKey, INT_32 &iOffset) const
{
	TokenHash_t iHash = oHashFun(oKey);
return FindElement(oKey, iHash, iOffset);
}

//
// Search element
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
  INT_32 Hash<TKey, TVal, THashFN, TCmpFN>::FindElement(const TKey & oKey, TokenHash_t iHash, INT_32 &iOffset) const
{
	// Offset
	iHash   = iHash & C_MAX_HASH_MASK;
	iOffset = iHash & pStorage -> bit_mask;

	// Find element
	while(pStorage -> hash_table[iOffset].hash & C_USED_CELL || pStorage -> hash_table[iOffset].hash & C_ERASED_CELL)
	{
		if ((pStorage -> hash_table[iOffset].hash & C_MAX_HASH_MASK) == iHash)
		{
			if (oCmpFun(pStorage -> hash_table[iOffset].data.first, oKey)) { return 1; }
		}
		// End of hash
		iOffset++;
		if (iOffset == pStorage -> max_size) { break; }
	}
// Okay
return 0;
}

//
// Auto expand
//
template <typename TKey, typename TVal, typename THashFN, typename TCmpFN>
  void Hash<TKey, TVal, THashFN, TCmpFN>::expand()
{
	INT_32 iNewSize    = pStorage -> max_size << 1;
	INT_32 iNewBitMask = iNewSize - 1;

	HashTable * aHashTable = new HashTable[iNewSize + 1];

	for (INT_32 iI = 0; iI < pStorage -> max_size; iI++)
	{
		// Copy ONLY used cells
		if (pStorage -> hash_table[iI].hash & C_USED_CELL)
		{
			INT_32 iOffset = pStorage -> hash_table[iI].hash & iNewBitMask;
			INT_32 iBaseOffset = iOffset;
			while(aHashTable[iOffset].hash & C_USED_CELL)
			{
				iOffset++;
				if (iOffset == pStorage -> max_size) { iOffset = 0; }
				if (iOffset == iBaseOffset) { throw "Ouch!"; }
			}
			aHashTable[iOffset].hash = pStorage -> hash_table[iI].hash | C_USED_CELL;
			aHashTable[iOffset].data = pStorage -> hash_table[iI].data;
		}
	}
	pStorage -> max_size = iNewSize;
	pStorage -> bit_mask = iNewBitMask;

	delete [] pStorage -> hash_table;
	pStorage -> hash_table = aHashTable;
}

} // namespace CTPP
#endif // _HASH_HPP__
// End.

