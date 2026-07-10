#include "glad/gl.h"
#include "engine3d/bufferlayout.hpp"

BufferElement::BufferElement(AttributeType type, uint8_t count, bool normalized)
    : type(type), normalized(normalized)
{
    if (count < 1 || count > 4)
    {
	this->count = 4;
    }
    else
    {
	this->count = count;
    }
}

unsigned int BufferElement::GetGLBaseType() const
{
    switch (type)
    {
	case AttributeType::Byte:
	    return GL_BYTE;
	case AttributeType::Int:
	    return GL_INT;
	case AttributeType::Float:
	    return GL_FLOAT;
    }

    return GL_FLOAT;
}

BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements)
    : m_elements(elements)
{
    // Calculate offsets and stride
    uint32_t offset = 0;
    for (BufferElement &element : m_elements)
    {
	element.offset = offset;

	unsigned int typeSizeBytes;
	switch (element.type)
	{
	    case AttributeType::Byte:
		typeSizeBytes = 1;
	    break;
	    case AttributeType::Int:
		typeSizeBytes = 4;
	    break;
	    case AttributeType::Float:
		typeSizeBytes = 4;
	    break;
	}

	offset += typeSizeBytes * element.count;
    }
    m_stride = offset;
}
