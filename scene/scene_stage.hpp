#ifndef PSYQ_SCENE_STAGE_HPP_
#define PSYQ_SCENE_STAGE_HPP_

//#include <psyq/scene/event_stage.hpp>

namespace psyq
{
	template< typename, typename, typename, typename > class scene_stage;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief scene�Ŏg��object��z�u�����B
 */
template<
	typename t_hash = psyq::fnv1_hash32,
	typename t_real = float,
	typename t_string = std::basic_string<
		char,
		std::char_traits< char >,
		psyq_extern::allocator::rebind< char >::other >,
	typename t_allocator = typename t_string::allocator_type >
class psyq::scene_stage
{
	public: typedef psyq::scene_stage< t_hash, t_real, t_string, t_allocator >
		this_type;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash;
	public: typedef t_real real;
	public: typedef t_string string;
	public: typedef t_allocator allocator;
	public: typedef psyq::event_stage< t_hash, t_real, t_string, t_allocator >
		event;
	public: typedef psyq::scene_camera<
		t_hash, t_real, typename t_string::const_pointer, t_allocator >
			camera;
	public: typedef typename this_type::camera::token token;

	//-------------------------------------------------------------------------
	/// scene-package�̎����B
	public: typedef std::map<
		typename t_hash::value,
		psyq::scene_package::shared_ptr,
		std::less< typename t_hash::value >,
		typename this_type::event::allocator::rebind<
			std::pair<
				typename t_hash::value const,
				psyq::scene_package::shared_ptr > >::other >
					package_map;

	/// scene-token�̎����B
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::token::shared_ptr,
		std::less< typename t_hash::value >,
		typename this_type::event::allocator::rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::token::shared_ptr > >::other >
					token_map;

	/// scene-camera�̎����B
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::camera::shared_ptr,
		std::less< typename t_hash::value >,
		typename this_type::event::allocator::rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::camera::shared_ptr > >::other >
					camera_map;

	//-------------------------------------------------------------------------
	public: class action:
		public this_type::event::action
	{
		protected: typedef
			psyq::scene_stage< t_hash, t_real, t_string, t_allocator >
				stage;
		public: typedef typename stage::action this_type;
		public: typedef typename stage::event::action super_type;

		public: class load_package;
		public: class load_token;
		public: class unload_token;
		public: class set_token_animation;
		public: class set_token_model;
		public: class set_section_camera;
		public: class set_section_light;
		public: class set_event_line;
		public: class set_time_scale;

		public: class apply_parameters:
			public this_type::stage::event::action::apply_parameters
		{
			public: typedef apply_parameters this_type;
			public: typedef typename stage::event::action::apply_parameters
				super_type;

			public: apply_parameters(
				stage&                              io_stage,
				typename stage::event::point const& i_point,
				typename stage::event::real const   i_time):
			super_type(i_point, i_time),
			stage_(io_stage)
			{
				// pass
			}

			public: stage& stage_;
		};
	};

	//-------------------------------------------------------------------------
	private: struct package_path
	{
		typename this_type::event::item::offset scene;   ///< scene��path���̏���offset�l�B
		typename this_type::event::item::offset shader;  ///< shader��path���̏���offset�l�B
		typename this_type::event::item::offset texture; ///< texture��path���̏���offset�l�B
	};

	private: typedef std::multimap<
		t_real,
		typename this_type::event::point const*,
		std::greater< t_real >,
		typename this_type::event::allocator::rebind<
			std::pair<
				t_real const,
				typename this_type::event::point const* > >::other >
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
	packages_(typename this_type::package_map::key_compare(), i_allocator),
	cameras_(typename this_type::camera_map::key_compare(), i_allocator),
	tokens_(typename this_type::token_map::key_compare(), i_allocator)
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
			typename this_type::event::line::scale::update_count(i_count);
			t_real const a_count(static_cast< t_real >(i_count));
			typename this_type::forward_scenes(this->tokens_, i_fps, a_count);

			// event���X�V�B
			typename this_type::dispatch_map a_dispatch(
				typename this_type::dispatch_map::key_compare(),
				this->event_.lines_.get_allocator());
			typename this_type::forward_events(
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
		typename t_hash::value const i_package)
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
		typename t_hash::value const i_package)
	const
	{
		return this_type::event::_find_element(this->packages_, i_package);
	}

	/** @brief scene-package���폜�B
	    @param[in] i_package �폜����scene-package�̖��Ohash�l�B
	    @return �폜����scene-package�B�폜���Ȃ������ꍇ�͋�B
	 */
	public: psyq::scene_package::shared_ptr erase_package(
		typename t_hash::value const i_package)
	{
		return this_type::event::_erase_element(this->packages_, i_package);
	}

	/** @brief file����scene-pacakge��ǂݍ��ށB
	    @param[in] i_package scene-package�̖��Ohash�l�B
	 */
	private: psyq::scene_package::shared_ptr load_package(
		typename t_hash::value const i_package)
	const
	{
		// event-package����scene-package-path�������B
		typename this_type::event::item const* const a_item(
			this_type::event::item::find(
				*this->event_.get_package(), i_package));
		if (NULL != a_item)
		{
			typename this_type::package_path const* const a_path(
				this->event_.get_address< typename this_type::package_path >(
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
	public: typename this_type::camera::shared_ptr const& get_camera(
		typename t_hash::value const i_camera)
	{
		if (t_hash::EMPTY != i_camera)
		{
			// ������camera���猟���B
			typename this_type::camera::shared_ptr& a_camera(
				this->cameras_[i_camera]);
			if (NULL != a_camera.get())
			{
				// camera�̎擾�ɐ����B
				return a_camera;
			}

			// �V����camera�����B
			PSYQ_ALLOCATE_SHARED< typename this_type::camera >(
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
		return psyq::_get_null_shared_ptr< typename this_type::camera >();
	}

	/** @brief camera�������B
	    @param[in] i_camera ��������camera�̖��Ohash�l�B
	    @return ��������camera�B������Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::camera::shared_ptr const& find_camera(
		typename t_hash::value const i_camera)
	const
	{
		return this_type::event::_find_element(this->cameras_, i_camera);
	}

	/** @brief camera���폜�B
	    @param[in] i_camera �폜����camera�̖��Ohash�l�B
	    @return �폜����camera�B�폜���Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::camera::shared_ptr erase_camera(
		typename t_hash::value const i_camera)
	{
		return this_type::event::_erase_element(this->cameras_, i_camera);
	}

	//-------------------------------------------------------------------------
	/** @brief stage����token���擾�B
	    token���ɑΉ�����token�����݂��Ȃ��ꍇ�́A�V����token�����B
	    @param[in] i_token �擾����token�̖��Ohash�l�B
	    @return token���ɑΉ�����token�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr const& get_token(
		typename t_hash::value const i_token)
	{
		if (t_hash::EMPTY != i_token)
		{
			// ������token���猟���B
			typename this_type::token::shared_ptr& a_token(
				this->tokens_[i_token]);
			if (NULL != a_token.get())
			{
				return a_token;
			}

			// �V����token�����B
			PSYQ_ALLOCATE_SHARED< typename this_type::token >(
				this->tokens_.get_allocator()).swap(a_token);
			if (NULL != a_token.get())
			{
				return a_token;
			}
			PSYQ_ASSERT(false);
			this->tokens_.erase(i_token);
		}
		return psyq::_get_null_shared_ptr< typename this_type::token >();
	}

	/** @brief camera����token���擾�B
	    token���ɑΉ�����token�����݂��Ȃ��ꍇ�́A�V����token�����B
	    camera���ɑΉ�����camera�����݂��Ȃ��ꍇ�́A�V����camera�����B
	    camera��token���Ȃ��ꍇ�́Acamera��token��ǉ�����B
	    @param[in] i_token  �擾����token�̖��Ohash�l�B
	    @param[in] i_camera �ΏۂƂȂ�camera�̖��Ohash�l�B
	    @return token���ɑΉ�����token�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr const& get_token(
		typename t_hash::value const i_token,
		typename t_hash::value const i_camera)
	{
		// token��camera���擾�B
		typename this_type::token::shared_ptr const& a_token(
			this->get_token(i_token));
		typename this_type::camera* const a_camera(
			this->get_camera(i_camera).get());

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
	public: typename this_type::token::shared_ptr const& find_token(
		typename t_hash::value const i_token)
	const
	{
		return this_type::event::_find_element(this->tokens_, i_token);
	}

	/** @brief stage��camera����token���폜�B
	    @param[in] i_token �폜����token�̖��Ohash�l�B
	    @return �폜����token�B�폜���Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr erase_token(
		typename t_hash::value const i_token)
	{
		typename this_type::token::shared_ptr a_token;

		// token���擾�B
		typename this_type::token_map::iterator const a_token_pos(
			this->tokens_.find(i_token));
		if (this->tokens_.end() != a_token_pos)
		{
			// stage����token���폜�B
			a_token.swap(a_token_pos->second);
			this->tokens_.erase(a_token_pos);

			// ���ׂĂ�camera����token���폜�B
			for (
				typename this_type::camera_map::const_iterator i =
					this->cameras_.begin();
				this->cameras_.end() != i;
				++i)
			{
				typename this_type::camera* const a_camera(i->second.get());
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
	public: typename this_type::token::shared_ptr const& erase_token(
		typename t_hash::value const i_token,
		typename t_hash::value const i_camera)
	{
		// camera�������B
		typename this_type::camera_map::const_iterator const a_camera_pos(
			this->cameras_.find(i_camera));
		if (this->cameras_.end() != a_camera_pos)
		{
			// token�������B
			typename this_type::token_map::const_iterator const a_token_pos(
				this->tokens_.find(i_token));
			if (this->tokens_.end() != a_token_pos)
			{
				// camera����Atoken���폜�B
				typename this_type::camera* const a_camera(
					a_camera_pos->second.get());
				if (NULL != a_camera &&
					a_camera->erase_token(a_token_pos->second))
				{
					return a_token_pos->second;
				}
			}
		}
		return psyq::_get_null_shared_ptr< typename this_type::token >();
	}

	//-------------------------------------------------------------------------
	/** @brief scene�̎��Ԃ��X�V�B
	    @param[in] i_tokens scene-token�����B
	    @param[in] i_fps    1�b�������frame���B
	    @param[in] i_count  �i�߂�frame���B
	 */
	private: static void forward_scenes(
		typename this_type::token_map const& i_tokens,
		t_real const                i_fps,
		t_real const                i_count)
	{
		for (
			typename this_type::token_map::const_iterator i = i_tokens.begin();
			i_tokens.end() != i;
			++i)
		{
			typename this_type::token* const a_token(i->second.get());
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
	private: static void update_scenes(
		typename this_type::token_map const& i_tokens)
	{
		for (
			typename this_type::token_map::const_iterator i = i_tokens.begin();
			i_tokens.end() != i;
			++i)
		{
			typename this_type::token* const a_token(i->second.get());
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
		typename this_type::dispatch_map&          io_dispatch,
		typename this_type::event::line_map const& i_lines,
		t_real const                      i_fps,
		t_real const                      i_count)
	{
		for (
			typename this_type::event::line_map::const_iterator i =
				i_lines.begin();
			i_lines.end() != i;
			++i)
		{
			// event-line�̎��Ԃ��X�V�B
			typename this_type::event::line_map::mapped_type& a_line(
				const_cast<
					typename this_type::event::line_map::mapped_type& >(
						i->second));
			a_line.seek(i_fps, i_count, SEEK_CUR);

			// ��������event��container�ɓo�^�B
			a_line._dispatch(io_dispatch);
		}
	}

	/** @brief container�ɓo�^����Ă���event�ɑΉ�����֐����Ăяo���B
	    @param[in] i_dispatch ��������event���o�^����Ă���container�B
	 */
	private: void apply_events(
		typename this_type::dispatch_map const& i_dispatch)
	{
		for (
			typename this_type::dispatch_map::const_iterator i =
				i_dispatch.begin();
			i_dispatch.end() != i;
			++i)
		{
			// event-point���擾�B
			PSYQ_ASSERT(NULL != i->second);
			typename this_type::event::point const& a_point(*i->second);

			// event-point�ɑΉ�����event�֐�object�������B
			typename this_type::event::action* const a_action(
				this_type::event::_find_element(
					this->event_.actions_, a_point.type).get());

			// event�֐�object��K�p�B
			if (NULL != a_action)
			{
				typename this_type::action::apply_parameters a_apply(
					*this, a_point, i->first);
				a_action->apply(a_apply);
			}
		}
	}

	//-------------------------------------------------------------------------
	public: typename this_type::event       event_;    ///< event�o�L��B
	public: typename this_type::package_map packages_; ///< scene-package�̎����B
	public: typename this_type::camera_map  cameras_;  ///< scene-camera�̎����B
	public: typename this_type::token_map   tokens_;   ///< scene-token�̎����B
};

//-----------------------------------------------------------------------------
namespace std
{
	template<
		typename t_hash,
		typename t_real,
		typename t_string,
		typename t_allocator >
	void swap(
		psyq::scene_stage< t_hash, t_real, t_string, t_allocator >& io_left,
		psyq::scene_stage< t_hash, t_real, t_string, t_allocator >& io_right)
	{
		io_left.swap(io_right);
	}
};

#endif // !PSYQ_SCENE_STAGE_HPP_
