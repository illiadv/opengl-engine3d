enum class BlendMode { None, Alpha };
enum class CullMode { None, Back };

struct RenderState
{
    bool depthTesting = true;
    BlendMode blending = BlendMode::None;
    CullMode culling = CullMode::Back;
};
