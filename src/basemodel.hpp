#ifndef BASEMODEL_HPP
#define BASEMODEL_HPP

class BaseModel
{
    public:
	BaseModel()
	{
	}
	virtual void Draw(unsigned int shader) = 0;
	virtual const char* GetDirectory() = 0;
};

#endif
