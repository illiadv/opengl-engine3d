#ifndef RENDERSTATE_HPP
#define RENDERSTATE_HPP

enum class BlendMode { None, Alpha };
enum class CullMode { None, Back };

struct RenderState
{
    bool depthTesting = true;
    BlendMode blending = BlendMode::None;
    CullMode culling = CullMode::Back;

    bool operator==(const RenderState &other) const
    {
	return (depthTesting == other.depthTesting) &&
	    (blending == other.blending) &&
	    (culling == other.culling);
    }
};

#endif
