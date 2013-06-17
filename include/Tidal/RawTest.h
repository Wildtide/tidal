#pragma once

#include <Tidal/Tidal.h>

class RawTest : public Frames::Raw
{
public:

	static RawTest *CreateBare(Layout *parent) { return new RawTest(parent); }
    static RawTest *CreateTagged_imp(const char *filename, int line, Layout *parent)
	{
		RawTest *rv = new RawTest(parent);
		rv->SetNameStatic(filename);
		rv->SetNameId(line);
		return rv;
	}

	static const char *GetStaticType() { return "RawTest"; }
	virtual const char *GetType() const { return GetStaticType(); }

private:

	GLuint ProgramID;
	GLuint vertexbuffer;
	GLuint MatrixID;
	GLuint MatrixIDMV;

	void Init();
	void Cleanup();

    RawTest(Layout *parent) : Frames::Raw(parent)
	{
		Init();
	}

    virtual ~RawTest()
	{
		Cleanup();
	}


	virtual void RenderElement(Frames::Renderer *renderer) const;
};
