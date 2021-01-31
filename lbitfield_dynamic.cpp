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

#include "lbitfield_dynamic.h"

#include <string.h>

namespace Lawn { // namespace start

void LBitField_Dynamic::initialize() {
	assert(0 && "LBitField_Dynamic : Does not support Initialize, use IT version");
}
void LBitField_Dynamic::terminate() {
	assert(0 && "LBitField_Dynamic : Does not support Terminate, use IT version");
}

void LBitField_Dynamic_IT::initialize() {
	initialize_do();
}

void LBitField_Dynamic_IT::terminate() {
	terminate_do();
}

void LBitField_Dynamic_IT::initialize_do() {
	memset(this, 0, sizeof(LBitField_Dynamic));
}

void LBitField_Dynamic_IT::terminate_do() {
	destroy();
}

void LBitField_Dynamic_IT::copy_from(const LBitField_Dynamic_IT &source) {
	create(source.get_num_bits(), false);
	memcpy(m_pucData, source.get_data(), source.get_num_bytes());
}

void LBitField_Dynamic_IT::create(unsigned int uiNumBits, bool bBlank) {
	// first delete any initial
	destroy();

	m_uiNumBits = uiNumBits;
	if (uiNumBits) {
		m_uiNumBytes = (uiNumBits / 8) + 1;

		m_pucData = new unsigned char[m_uiNumBytes];

		if (bBlank)
			blank(false);
	}
}

void LBitField_Dynamic_IT::destroy() {
	if (m_pucData) {
		delete[] m_pucData;
		m_pucData = 0;
	}

	memset(this, 0, sizeof(LBitField_Dynamic));
}

void LBitField_Dynamic_IT::blank(bool bSetOrZero) {
	if (bSetOrZero) {
		memset(m_pucData, 255, m_uiNumBytes);
	} else {
		memset(m_pucData, 0, m_uiNumBytes);
	}
}

void LBitField_Dynamic_IT::invert() {
	for (unsigned int n = 0; n < m_uiNumBytes; n++) {
		m_pucData[n] = ~m_pucData[n];
	}
}

////////////////////////////////////////////////////////////////////////////

} // namespace Lawn
