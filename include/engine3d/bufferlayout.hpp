#include <stdint.h>
#include <vector>

namespace e3d
{

enum class AttributeType {
    Byte, Int, Float
};

struct BufferElement {
    AttributeType type;
    uint8_t count;
    bool normalized;
    uint32_t offset;

    BufferElement(AttributeType type, uint8_t count, bool normalized = false);
    unsigned int GetGLBaseType() const;
};

class BufferLayout {
public:
    BufferLayout(std::initializer_list<BufferElement> elements);
    inline const std::vector<BufferElement>& GetElements() const { return m_elements; }
    inline uint32_t GetStride() const { return m_stride; }

private:
    std::vector<BufferElement> m_elements;
    uint32_t m_stride;
};

} // End namespace e3d
