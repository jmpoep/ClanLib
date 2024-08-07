/*
**  ClanLib SDK
**  Copyright (c) 1997-2020 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    Harry Storbacka
**    Kenneth Gangstoe
*/

#include "GL/precomp.h"
#include "gl3_shader_object_provider.h"
#include "gl3_graphic_context_provider.h"
#include "API/Core/System/exception.h"
#include "API/Core/Text/string_help.h"
#include "API/Core/Text/string_format.h"
#include "API/Display/Render/shared_gc_data.h"

namespace clan
{
	GL3ShaderObjectProvider::GL3ShaderObjectProvider()
		: handle(0)
	{
		SharedGCData::add_disposable(this);
	}
	void GL3ShaderObjectProvider::create(
		ShaderType shader_type,
		const void *source, int source_size)
	{
		throw Exception("Fixme");
	}

	void GL3ShaderObjectProvider::create(
		ShaderType shader_type,
		const std::string &source)
	{
		OpenGL::set_active();
		if (handle)
			glDeleteShader(handle);

		type = shader_type;
		handle = glCreateShader(shadertype_to_opengl(type));

		const GLchar *sources[1];
		GLint source_lengths[1];
		source_lengths[0] = source.length();
		sources[0] = source.c_str();
		glShaderSource(handle, 1, sources, source_lengths);
	}

	void GL3ShaderObjectProvider::create(
		ShaderType shader_type,
		const std::vector<std::string> &sources)
	{
		OpenGL::set_active();
		if (handle)
			glDeleteShader(handle);

		type = shader_type;
		handle = glCreateShader(shadertype_to_opengl(type));

		std::vector<GLint> array_lengths(sources.size());
		std::vector<GLchar*> array_sources(sources.size());
		for (size_t i = 0; i < sources.size(); i++)
		{
		    array_lengths[i] = sources[i].length();
		    array_sources[i] = (GLchar*)sources[i].c_str();
		}
		glShaderSource(handle, sources.size(), array_sources.data(), array_lengths.data());
	}

	GL3ShaderObjectProvider::~GL3ShaderObjectProvider()
	{
		dispose();
		SharedGCData::remove_disposable(this);
	}

	void GL3ShaderObjectProvider::on_dispose()
	{
		if (handle)
		{
			if (OpenGL::set_active())
			{
				glDeleteShader(handle);
			}
		}
	}

	unsigned int GL3ShaderObjectProvider::get_handle() const
	{
		return (unsigned int)handle;
	}

	bool GL3ShaderObjectProvider::get_compile_status() const
	{
		OpenGL::set_active();
		GLint status = 0;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
		return (status != GL_FALSE);
	}

	ShaderType GL3ShaderObjectProvider::get_shader_type() const
	{
		return type;
	}

	std::string GL3ShaderObjectProvider::get_info_log() const
	{
		OpenGL::set_active();
		std::string result;
		GLsizei buffer_size = 16 * 1024;
		while (buffer_size < 2 * 1024 * 1024)
		{
			auto info_log = new GLchar[buffer_size];
			GLsizei length = 0;
			glGetShaderInfoLog(handle, buffer_size, &length, info_log);
			if (length < buffer_size - 1)
				result = std::string(info_log, length);
			delete[] info_log;
			if (length < buffer_size - 1)
				break;
			buffer_size *= 2;
		}
		return result;
	}

	std::string GL3ShaderObjectProvider::get_shader_source() const
	{
		OpenGL::set_active();
		std::string result;
		GLsizei buffer_size = 16 * 1024;
		while (buffer_size < 2 * 1024 * 1024)
		{
			auto shader_source = new GLchar[buffer_size];
			GLsizei length = 0;
			glGetShaderSource(handle, buffer_size, &length, shader_source);
			if (length < buffer_size - 1)
				result = std::string(shader_source, length);
			delete[] shader_source;
			if (length < buffer_size - 1)
				break;
			buffer_size *= 2;
		}
		return result;
	}

	void GL3ShaderObjectProvider::compile()
	{
		OpenGL::set_active();
		glCompileShader(handle);
	}

	GLenum GL3ShaderObjectProvider::shadertype_to_opengl(ShaderType type)
	{
		switch (type)
		{
		case ShaderType::vertex:
			return GL_VERTEX_SHADER;
		case ShaderType::geometry:
			return GL_GEOMETRY_SHADER;
		case ShaderType::fragment:
			return GL_FRAGMENT_SHADER;
		case ShaderType::tess_evaluation:
			return GL_TESS_EVALUATION_SHADER;
		case ShaderType::tess_control:
			return GL_TESS_CONTROL_SHADER;
		case ShaderType::compute:
			return GL_COMPUTE_SHADER;
		default:
			throw Exception(string_format("GL3ShaderObjectProvider: Unknown shader type: %1", (int)type));
		}
	}
}
