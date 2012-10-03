#ifndef PSYQ_SCENE_WORLD_HPP_
#define PSYQ_SCENE_WORLD_HPP_

//#include <psyq/scene/event_stage.hpp>

namespace psyq
{
	class scene_stage;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief scene�Ŏg��object��z�u�����B
 */
class psyq::scene_stage
{
	typedef psyq::scene_stage this_type;

	//-------------------------------------------------------------------------
	private: typedef psyq::fnv1_hash32 t_hash;
	private: typedef float t_real;
	private: typedef std::basic_string<
		char,
		std::char_traits< char >,
		psyq_extern::allocator::rebind< char >::other >
			t_string;
	private: typedef t_string::allocator_type t_allocator;

	//-------------------------------------------------------------------------
	public: typedef psyq::event_stage< t_hash, t_real, t_string, t_allocator >
		event;
	public: typedef psyq::scene_camera<
		t_hash, t_real, t_string::const_pointer, t_allocator >
			camera;
	public: typedef this_type::camera::token token;

	//-------------------------------------------------------------------------
	/// scene-package�̎����B
	public: typedef std::map<
		t_hash::value,
		psyq::scene_package::shared_ptr,
		std::less< t_hash::value >,
		this_type::event::allocator::rebind<
			std::pair<
				t_hash::value const,
				psyq::scene_package::shared_ptr > >::other >
					package_map;

	/// scene-token�̎����B
	public: typedef std::map<
		t_hash::value,
		this_type::token::shared_ptr,
		std::less< t_hash::value >,
		this_type::event::allocator::rebind<
			std::pair<
				t_hash::value const,
				this_type::token::shared_ptr > >::other >
					token_map;

	/// scene-camera�̎����B
	public: typedef std::map<
		t_hash::value,
		this_type::camera::shared_ptr,
		std::less< t_hash::value >,
		this_type::event::allocator::rebind<
			std::pair<
				t_hash::value const,
				this_type::camera::shared_ptr > >::other >
					camera_map;

	//-------------------------------------------------------------------------
	public: class event_apply_parameters:
		public this_type::event::action::apply_parameters
	{
		typedef event_apply_parameters this_type;
		typedef psyq::scene_stage::event::action::apply_parameters super_type;

		public: event_apply_parameters(
			psyq::scene_stage&                         io_stage,
			psyq::event_point< t_hash, t_real > const& i_point,
			t_real const                               i_time):
		super_type(i_point, i_time),
		stage_(io_stage)
		{
			// pass
		}

		public: psyq::scene_stage& stage_;
	};

	//-------------------------------------------------------------------------
	private: struct package_path
	{
		this_type::event::item::offset scene;   ///< scene��path���̏���offset�l�B
		this_type::event::item::offset shader;  ///< shader��path���̏���offset�l�B
		this_type::event::item::offset texture; ///< texture��path���̏���offset�l�B
	};

	private: typedef std::multimap<
		t_real,
		this_type::event::point const*,
		std::greater< t_real >,
		this_type::event::allocator::rebind<
			std::pair<
				t_real const,
				this_type::event::point const* > >::other >
					dispatch_map;

	//-------------------------------------------------------------------------
	/** @param[in] i_package   �g�p����event-package�B
	    @param[in] i_allocator �������Ɏg�p����memory�����q�B
	 */
	public: template< typename t_allocator >
	scene_stage(
		PSYQ_SHARED_PTR< psyq::event_package const > const& i_package,
		t_allocator const&                                  i_allocator):
	event_(i_package, i_allocator),
	packages_(this_type::package_map::key_compare(), i_allocator),
	cameras_(this_type::camera_map::key_compare(), i_allocator),
	tokens_(this_type::token_map::key_compare(), i_allocator)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief stage�S�̂������B
	    @param[in,out] io_target ��������stage�S�́B
	 */
	public: void swap(this_type& io_target)
	{
		this->event_.swap(io_target.event_);
		this->packages_.swap(io_target.packages_);
		this->cameras_.swap(io_target.cameras_);
		this->tokens_.swap(io_target.tokens_);
	}

	//-------------------------------------------------------------------------
	/** @brief �X�V�B
	    @param[in] i_fps   1�b�������frame���B
	    @param[in] i_count �i�߂�frame���B
	 */
	public: void update(t_real const i_fps, unsigned const i_count = 1)
	{
		if (0 < i_fps)
		{
			// scene�̎��Ԃ��X�V�B
			this_type::event::line::scale::update_count(i_count);
			t_real const a_count(static_cast< t_real >(i_count));
			this_type::forward_scenes(this->tokens_, i_fps, a_count);

			// event���X�V�B
			this_type::dispatch_map a_dispatch(
				this_type::dispatch_map::key_compare(),
				this->event_.lines_.get_allocator());
			this_type::forward_events(
				a_dispatch, this->event_.lines_, i_fps, a_count);
			this->apply_events(a_dispatch);

			// scene���X�V�B
			this_type::update_scenes(this->tokens_);
		}
	}

	//-------------------------------------------------------------------------
	/** @brief scene-package���擾�B
	    package���ɑΉ�����scene-package�����݂��Ȃ��ꍇ�́A
	    file����ǂݍ���Œǉ�����B
	    @param[in] i_name �擾����scene-package�̖��Ohash�l�B
	    @return package���ɑΉ�����scene-package�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: psyq::scene_package::shared_ptr const& get_package(
		t_hash::value const i_package)
	{
		if (t_hash::EMPTY != i_package)
		{
			// ������scene-packae�������B
			psyq::scene_package::shared_ptr& a_package(
				this->packages_[i_package]);
			if (NULL != a_package.get())
			{
				// scene-package�̎擾�ɐ����B
				return a_package;
			}

			// file����scene-package��ǂݍ��ށB
			this->load_package(i_package).swap(a_package);
			if (NULL != a_package.get())
			{
				// scene-package�̎擾�ɐ����B
				return a_package;
			}
			PSYQ_ASSERT(false);
			this->packages_.erase(i_package);
		}

		// scene-package�̎擾�Ɏ��s�B
		return psyq::_get_null_shared_ptr< psyq::scene_package >();
	}

	/** @brief scene-package�������B
	    @param[in] i_package ��������scene-package�̖��Ohash�l�B
	    @return ������scene-pacakge�B������Ȃ������ꍇ�͋�B
	 */
	public: psyq::scene_package::shared_ptr const& find_package(
		t_hash::value const i_package)
	const
	{
		return this_type::event::_find_element(this->packages_, i_package);
	}

	/** @brief scene-package���폜�B
	    @param[in] i_package �폜����scene-package�̖��Ohash�l�B
	    @return �폜����scene-package�B�폜���Ȃ������ꍇ�͋�B
	 */
	public: psyq::scene_package::shared_ptr erase_package(
		t_hash::value const i_package)
	{
		return this_type::event::_erase_element(this->packages_, i_package);
	}

	/** @brief file����scene-pacakge��ǂݍ��ށB
	    @param[in] i_package scene-package�̖��Ohash�l�B
	 */
	private: psyq::scene_package::shared_ptr load_package(
		t_hash::value const i_package)
	const
	{
		// event-package����scene-package-path�������B
		this_type::event::item const* const a_item(
			this_type::event::item::find(
				*this->event_.get_package(), i_package));
		if (NULL != a_item)
		{
			this_type::package_path const* const a_path(
				this->event_.get_address< this_type::package_path >(
					a_item->begin));
			if (NULL != a_path)
			{
				// file����scene-package��ǂݍ��ށB
				psyq::scene_package::shared_ptr const a_package(
					psyq::scene_package::load(
						this->packages_.get_allocator(),
						this->event_.replace_string(a_path->scene),
						this->event_.replace_string(a_path->shader),
						this->event_.replace_string(a_path->texture)));
				if (NULL != a_package.get())
				{
					return a_package;
				}
			}
		}
		return psyq::_get_null_shared_ptr< psyq::scene_package >();
	}

	//-------------------------------------------------------------------------
	/** @brief camera���擾�B
	    camera���ɑΉ�����camera�����݂��Ȃ��ꍇ�́A�V����camera�����B
	    @param[in] i_camera �擾����camera�̖��Ohash�l�B
	    @return camera���ɑΉ�����camera�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: this_type::camera::shared_ptr const& get_camera(
		t_hash::value const i_camera)
	{
		if (t_hash::EMPTY != i_camera)
		{
			// ������camera���猟���B
			this_type::camera::shared_ptr& a_camera(
				this->cameras_[i_camera]);
			if (NULL != a_camera.get())
			{
				// camera�̎擾�ɐ����B
				return a_camera;
			}

			// �V����camera�����B
			PSYQ_ALLOCATE_SHARED< this_type::camera >(
				this->cameras_.get_allocator(),
				this->cameras_.get_allocator()).swap(a_camera);
			if (NULL != a_camera.get())
			{
				// camera�̎擾�ɐ����B
				return a_camera;
			}
			PSYQ_ASSERT(false);
			this->cameras_.erase(i_camera);
		}

		// camera�̎擾�Ɏ��s�B
		return psyq::_get_null_shared_ptr< this_type::camera >();
	}

	/** @brief camera�������B
	    @param[in] i_camera ��������camera�̖��Ohash�l�B
	    @return ��������camera�B������Ȃ������ꍇ�͋�B
	 */
	public: this_type::camera::shared_ptr const& find_camera(
		t_hash::value const i_camera)
	const
	{
		return this_type::event::_find_element(this->cameras_, i_camera);
	}

	/** @brief camera���폜�B
	    @param[in] i_camera �폜����camera�̖��Ohash�l�B
	    @return �폜����camera�B�폜���Ȃ������ꍇ�͋�B
	 */
	public: this_type::camera::shared_ptr erase_camera(
		t_hash::value const i_camera)
	{
		return this_type::event::_erase_element(this->cameras_, i_camera);
	}

	//-------------------------------------------------------------------------
	/** @brief stage����token���擾�B
	    token���ɑΉ�����token�����݂��Ȃ��ꍇ�́A�V����token�����B
	    @param[in] i_token �擾����token�̖��Ohash�l�B
	    @return token���ɑΉ�����token�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: this_type::token::shared_ptr const& get_token(
		t_hash::value const i_token)
	{
		if (t_hash::EMPTY != i_token)
		{
			// ������token���猟���B
			this_type::token::shared_ptr& a_token(this->tokens_[i_token]);
			if (NULL != a_token.get())
			{
				return a_token;
			}

			// �V����token�����B
			PSYQ_ALLOCATE_SHARED< this_type::token >(
				this->tokens_.get_allocator()).swap(a_token);
			if (NULL != a_token.get())
			{
				return a_token;
			}
			PSYQ_ASSERT(false);
			this->tokens_.erase(i_token);
		}
		return psyq::_get_null_shared_ptr< this_type::token >();
	}

	/** @brief camera����token���擾�B
	    token���ɑΉ�����token�����݂��Ȃ��ꍇ�́A�V����token�����B
	    camera���ɑΉ�����camera�����݂��Ȃ��ꍇ�́A�V����camera�����B
	    camera��token���Ȃ��ꍇ�́Acamera��token��ǉ�����B
	    @param[in] i_token  �擾����token�̖��Ohash�l�B
	    @param[in] i_camera �ΏۂƂȂ�camera�̖��Ohash�l�B
	    @return token���ɑΉ�����token�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: this_type::token::shared_ptr const& get_token(
		t_hash::value const i_token,
		t_hash::value const i_camera)
	{
		// token��camera���擾�B
		this_type::token::shared_ptr const& a_token(this->get_token(i_token));
		this_type::camera* const a_camera(this->get_camera(i_camera).get());

		// token��camera�ɒǉ��B
		if (NULL != a_camera)
		{
			a_camera->insert_token(a_token);
		}
		return a_token;
	}

	/** @brief stage����token�������B
	    @param[in] i_token ��������token�̖��Ohash�l�B
	    @return ������token�B������Ȃ������ꍇ�͋�B
	 */
	public: this_type::token::shared_ptr const& find_token(
		t_hash::value const i_token)
	const
	{
		return this_type::event::_find_element(this->tokens_, i_token);
	}

	/** @brief stage��camera����token���폜�B
	    @param[in] i_token �폜����token�̖��Ohash�l�B
	    @return �폜����token�B�폜���Ȃ������ꍇ�͋�B
	 */
	public: this_type::token::shared_ptr erase_token(
		t_hash::value const i_token)
	{
		this_type::token::shared_ptr a_token;

		// token���擾�B
		this_type::token_map::iterator const a_token_pos(
			this->tokens_.find(i_token));
		if (this->tokens_.end() != a_token_pos)
		{
			// stage����token���폜�B
			a_token.swap(a_token_pos->second);
			this->tokens_.erase(a_token_pos);

			// ���ׂĂ�camera����token���폜�B
			for (
				this_type::camera_map::const_iterator i =
					this->cameras_.begin();
				this->cameras_.end() != i;
				++i)
			{
				this_type::camera* const a_camera(i->second.get());
				if (NULL != a_camera)
				{
					a_camera->erase_token(a_token);
				}
			}
		}
		return a_token;
	}

	/** @brief camera����token���폜�B
	    @param[in] i_token  �폜����token�̖��Ohash�l�B
	    @param[in] i_camera �ΏۂƂȂ�camera�̖��Ohash�l�B
	    @return �폜����token�B�폜���Ȃ������ꍇ�͋�B
	 */
	public: this_type::token::shared_ptr const& erase_token(
		t_hash::value const i_token,
		t_hash::value const i_camera)
	{
		// camera�������B
		this_type::camera_map::const_iterator const a_camera_pos(
			this->cameras_.find(i_camera));
		if (this->cameras_.end() != a_camera_pos)
		{
			// token�������B
			this_type::token_map::const_iterator const a_token_pos(
				this->tokens_.find(i_token));
			if (this->tokens_.end() != a_token_pos)
			{
				// camera����Atoken���폜�B
				this_type::camera* const a_camera(a_camera_pos->second.get());
				if (NULL != a_camera &&
					a_camera->erase_token(a_token_pos->second))
				{
					return a_token_pos->second;
				}
			}
		}
		return psyq::_get_null_shared_ptr< this_type::token >();
	}

	//-------------------------------------------------------------------------
	/** @brief scene�̎��Ԃ��X�V�B
	    @param[in] i_tokens scene-token�����B
	    @param[in] i_fps    1�b�������frame���B
	    @param[in] i_count  �i�߂�frame���B
	 */
	private: static void forward_scenes(
		this_type::token_map const& i_tokens,
		t_real const                i_fps,
		t_real const                i_count)
	{
		for (
			this_type::token_map::const_iterator i = i_tokens.begin();
			i_tokens.end() != i;
			++i)
		{
			this_type::token* const a_token(i->second.get());
			if (NULL != a_token)
			{
				psyq_extern::forward_scene_unit(
					a_token->scene_,
					i_fps,
					this_type::event::line::scale::get_scale(
						a_token->time_scale_, i_count));
			}
		}
	}

	/** @brief scene���X�V�B
	    @param[in] i_tokens scene������token�̎����B
	 */
	private: static void update_scenes(this_type::token_map const& i_tokens)
	{
		for (
			this_type::token_map::const_iterator i = i_tokens.begin();
			i_tokens.end() != i;
			++i)
		{
			this_type::token* const a_token(i->second.get());
			if (NULL != a_token)
			{
				psyq_extern::update_scene_unit(a_token->scene_);
			}
		}
	}

	//-------------------------------------------------------------------------
	/** @brief event-line�̎��Ԃ��X�V���A��������event��container�ɓo�^�B
	    @param[in,out] io_dispatch ��������event��o�^����container�B
	    @param[in]     i_lines     �X�V����event-line�̎����B
	    @param[in]     i_fps       1�b�������frame���B
	    @param[in]     i_count     �i�߂�frame���B
	 */
	private: static void forward_events(
		this_type::dispatch_map&          io_dispatch,
		this_type::event::line_map const& i_lines,
		t_real const                      i_fps,
		t_real const                      i_count)
	{
		for (
			this_type::event::line_map::const_iterator i = i_lines.begin();
			i_lines.end() != i;
			++i)
		{
			// event-line�̎��Ԃ��X�V�B
			this_type::event::line_map::mapped_type& a_line(
				const_cast< this_type::event::line_map::mapped_type& >(
					i->second));
			a_line.seek(i_fps, i_count, SEEK_CUR);

			// ��������event��container�ɓo�^�B
			a_line._dispatch(io_dispatch);
		}
	}

	/** @brief container�ɓo�^����Ă���event�ɑΉ�����֐����Ăяo���B
	    @param[in] i_dispatch ��������event���o�^����Ă���container�B
	 */
	private: void apply_events(this_type::dispatch_map const& i_dispatch)
	{
		for (
			this_type::dispatch_map::const_iterator i = i_dispatch.begin();
			i_dispatch.end() != i;
			++i)
		{
			// event-point���擾�B
			PSYQ_ASSERT(NULL != i->second);
			this_type::event::point const& a_point(*i->second);

			// event-point�ɑΉ�����event�֐�object�������B
			this_type::event::action* const a_action(
				this_type::event::_find_element(
					this->event_.actions_, a_point.type).get());

			// event�֐�object��K�p�B
			if (NULL != a_action)
			{
				this_type::event_apply_parameters a_apply(*this, a_point, i->first);
				a_action->apply(a_apply);
			}
		}
	}

	//-------------------------------------------------------------------------
	public: this_type::event       event_;    ///< event�o�L��B
	public: this_type::package_map packages_; ///< scene-package�̎����B
	public: this_type::camera_map  cameras_;  ///< scene-camera�̎����B
	public: this_type::token_map   tokens_;   ///< scene-token�̎����B
};

//-----------------------------------------------------------------------------
namespace std
{
	void swap(psyq::scene_stage& io_left, psyq::scene_stage& io_right)
	{
		io_left.swap(io_right);
	}
};

#endif // !PSYQ_SCENE_WORLD_HPP_
