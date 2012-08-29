#ifndef PSYQ_SCENE_TOKEN_HPP_
#define PSYQ_SCENE_TOKEN_HPP_

namespace psyq
{
	class texture_package;
	class shader_package;
	class scene_package;
	class scene_token;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::texture_package
{
	typedef psyq::texture_package this_type;

	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;
	public: typedef PSYQ_SHARED_PTR< this_type const > const_shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type const > const_weak_ptr;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::shader_package
{
	typedef psyq::shader_package this_type;

	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;
	public: typedef PSYQ_SHARED_PTR< this_type const > const_shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type const > const_weak_ptr;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::scene_package
{
	typedef psyq::scene_package this_type;

	//-------------------------------------------------------------------------
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;
	public: typedef PSYQ_SHARED_PTR< this_type const > const_shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type const > const_weak_ptr;

	//-------------------------------------------------------------------------
	public: template< typename t_allocator >
	static this_type::shared_ptr load(
		t_allocator const&               i_allocator,
		psyq::scene_event::string const& i_scene_path,
		psyq::scene_event::string const& i_shader_path,
		psyq::scene_event::string const& i_texture_path)
	{
		if (!i_scene_path.empty())
		{
			// texture��file����ǂݍ��ށB
			psyq::texture_package::shared_ptr a_texture;
			if (!i_texture_path.empty())
			{
				typename t_allocator::template
					rebind< psyq::texture_package >::other a_allocator(
						i_allocator);
				this_type::load_file(
					a_allocator, i_texture_path).swap(a_texture);
				if (NULL == a_texture.get())
				{
					// texture���ǂݍ��߂Ȃ������B
					PSYQ_ASSERT(false);
					return this_type::shared_ptr();
				}
			}

			// shader��file����ǂݍ��ށB
			psyq::shader_package::shared_ptr a_shader;
			if (!i_shader_path.empty())
			{
				typename t_allocator::template
					rebind< psyq::shader_package >::other a_allocator(
						i_allocator);
				this_type::load_file(
					a_allocator, i_shader_path).swap(a_shader);
				if (NULL == a_shader.get())
				{
					// shader���ǂݍ��߂Ȃ������B
					PSYQ_ASSERT(false);
					return this_type::shared_ptr();
				}
			}

			// scene��file����ǂݍ��ށB
			typename t_allocator::template
				rebind< psyq::scene_package >::other a_allocator(i_allocator);
			this_type::shared_ptr const a_scene(
				this_type::load_file(a_allocator, i_scene_path));
			if (NULL != a_scene.get())
			{
				a_scene->shader_.swap(a_shader);
				a_scene->texture_.swap(a_texture);
				return a_scene;
			}
			else
			{
				PSYQ_ASSERT(false);
			}
		}
		return this_type::shared_ptr();
	}

	//-------------------------------------------------------------------------
	private: template< typename t_allocator, typename t_string >
	static PSYQ_SHARED_PTR< typename t_allocator::value_type > load_file(
		t_allocator&    io_allocator,
		t_string const& i_path)
	{
		io_allocator.max_size();
		i_path.length();

		// �������Ȃ̂ŁB
		return PSYQ_SHARED_PTR< typename t_allocator::value_type >();
	}

	//-------------------------------------------------------------------------
	private: psyq::shader_package::shared_ptr  shader_;
	private: psyq::texture_package::shared_ptr texture_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::scene_token
{
	typedef psyq::scene_token this_type;

	//-------------------------------------------------------------------------
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;
	public: typedef PSYQ_SHARED_PTR< this_type const > const_shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type const > const_weak_ptr;

	//-------------------------------------------------------------------------
	public: psyq_extern::scene_unit                   scene_;
	public: psyq::scene_event::time_scale::shared_ptr time_scale_;
};

#endif // !PSYQ_SCENE_TOKEN_HPP_
