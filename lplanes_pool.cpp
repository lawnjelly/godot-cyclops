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

#include "lplanes_pool.h"

using namespace Lawn;

LPlanesPool::LPlanesPool() {
	reset();

	// preallocate the vectors to a reasonable size
	for (int n = 0; n < POOL_MAX; n++) {
		_planes[n].resize(32);
	}
}

void LPlanesPool::reset() {
	for (int n = 0; n < POOL_MAX; n++) {
		_freelist[n] = POOL_MAX - n - 1;
	}

	_num_free = POOL_MAX;
}

unsigned int LPlanesPool::request() {
	if (!_num_free)
		return -1;

	_num_free--;
	return _freelist[_num_free];
}

void LPlanesPool::free(unsigned int ui) {
	assert(ui <= POOL_MAX);
	assert(_num_free < POOL_MAX);

	_freelist[_num_free] = ui;
	_num_free++;
}
