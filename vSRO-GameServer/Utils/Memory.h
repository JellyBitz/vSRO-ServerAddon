#pragma once

template<typename T>
static auto call_virtual(void* base_ptr, unsigned index) -> T
{
	return (*static_cast<T**>(base_ptr))[index];
}