#pragma once

//	Copyright (c) 2021 Lawnjelly

//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:

//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.

//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

#include <assert.h>

namespace Lawn { // namespace start

class LBitField_Dynamic_IT {
public:
	// construction
	void initialize();
	void terminate();

private:
	// prevent copying (see effective C++ scott meyers)
	// there is no implementation for copy constructor, hence compiler will complain if you try to copy.
	LBitField_Dynamic_IT &operator=(const LBitField_Dynamic_IT &);

public:
	// create automatically blanks
	void create(unsigned int uiNumBits, bool bBlank = true);
	void destroy();

	// public funcs
	inline unsigned int get_num_bits() const { return m_uiNumBits; }
	inline unsigned int get_bit(unsigned int uiBit) const;
	inline void set_bit(unsigned int uiBit, unsigned int bSet);
	bool check_and_set(unsigned int uiBit);
	void blank(bool bSetOrZero = false);
	void invert();
	void copy_from(const LBitField_Dynamic_IT &source);

	// loading / saving
	unsigned char *get_data() { return m_pucData; }
	const unsigned char *get_data() const { return m_pucData; }
	unsigned int get_num_bytes() const { return m_uiNumBytes; }

protected:
	// member funcs
	void initialize_do();
	void terminate_do();

	// member vars
	unsigned char *m_pucData;
	unsigned int m_uiNumBytes;
	unsigned int m_uiNumBits;
};

class LBitField_Dynamic : public LBitField_Dynamic_IT {
public:
	// call initialize and terminate automatically
	LBitField_Dynamic(unsigned int uiNumBits) {
		initialize_do();
		create(uiNumBits);
	}
	LBitField_Dynamic() { initialize_do(); }
	~LBitField_Dynamic() { terminate_do(); }

	// disallow explicit calls
	void initialize();
	void terminate();
};

//////////////////////////////////////////////////////////
inline unsigned int LBitField_Dynamic_IT::get_bit(unsigned int uiBit) const {
	assert(m_pucData);
	unsigned int uiByteNumber = uiBit >> 3; // divide by 8
	assert(uiByteNumber < m_uiNumBytes);
	unsigned char uc = m_pucData[uiByteNumber];
	unsigned int uiBitSet = uc & (1 << (uiBit & 7));
	return uiBitSet;
}

inline bool LBitField_Dynamic_IT::check_and_set(unsigned int uiBit) {
	assert(m_pucData);
	unsigned int uiByteNumber = uiBit >> 3; // divide by 8
	assert(uiByteNumber < m_uiNumBytes);
	unsigned char &uc = m_pucData[uiByteNumber];
	unsigned int uiMask = (1 << (uiBit & 7));
	unsigned int uiBitSet = uc & uiMask;
	if (uiBitSet)
		return false;

	// set
	uc = uc | uiMask;
	return true;
}

inline void LBitField_Dynamic_IT::set_bit(unsigned int uiBit, unsigned int bSet) {
	assert(m_pucData);
	unsigned int uiByteNumber = uiBit >> 3; // divide by 8
	assert(uiByteNumber < m_uiNumBytes);
	unsigned char uc = m_pucData[uiByteNumber];
	unsigned int uiMask = 1 << (uiBit & 7);
	if (bSet) {
		uc = uc | uiMask;
	} else {
		uc &= ~uiMask;
	}
	m_pucData[uiByteNumber] = uc;
}

} // namespace Lawn
