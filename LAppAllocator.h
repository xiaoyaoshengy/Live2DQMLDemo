#pragma once

#include "Framework/CubismFramework.hpp"
#include "Framework/ICubismAllocator.hpp"

class LAppAllocator : public Csm::ICubismAllocator
{
	void* Allocate(const Csm::csmSizeType size);
	void Deallocate(void* memory);
	void* AllocateAligned(const Csm::csmSizeType size, const Csm::csmUint32 alignment);
	void DeallocateAligned(void* alignedMemory);
};

