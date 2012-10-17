#ifndef PSYQ_SCENE_STAGE_HPP_
#define PSYQ_SCENE_STAGE_HPP_

//#include <psyq/scene/event_stage.hpp>
//#include <psyq/scene/scene_action.hpp>
//#include <psyq/scene/scene_screen.hpp>

namespace psyq
{
	template< typename, typename, typename, typename > class scene_stage;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief scene�Ŏg��object��z�u�����B
    @tparam t_hash      event-package�Ŏg���Ă���hash�֐��B
    @tparam t_real      event-package�Ŏg���Ă�������̌^�B
    @tparam t_string    event�u����Ɏg��������̌^�Bstd::basic_string�݊��B
    @tparam t_allocator �g�p����memory�����q�̌^�B
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
	public: typedef psyq::scene_screen<
		t_hash, t_real, typename t_string::const_pointer, t_allocator >
			screen;
	public: typedef typename this_type::screen::token token;
	public: typedef typename this_type::event::const_string const_string;

	//-------------------------------------------------------------------------
	/// scene-package�̎����B
	public: typedef std::map<
		typename t_hash::value,
		psyq::scene_package::shared_ptr,
		std::less< typename t_hash::value >,
		typename this_type::event::allocator::template rebind<
			std::pair<
				typename t_hash::value const,
				psyq::scene_package::shared_ptr > >::other >
					package_map;

	/// scene-token�̎����B
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::token::shared_ptr,
		std::less< typename t_hash::value >,
		typename this_type::event::allocator::template rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::token::shared_ptr > >::other >
					token_map;

	/// scene-screen�̎����B
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::screen::shared_ptr,
		std::less< typename t_hash::value >,
		typename this_type::event::allocator::template rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::screen::shared_ptr > >::other >
					screen_map;

	//-------------------------------------------------------------------------
	private: struct package_path
	{
		/// scene��path����package-offset�l�B
		typename this_type::event::package::offset scene;

		/// shader��path����package-offset�l�B
		typename this_type::event::package::offset shader;

		/// texture��path����package-offset�l�B
		typename this_type::event::package::offset texture;
	};

	private: typedef std::multimap<
		t_real,
		typename this_type::event::action::point const*,
		std::greater< t_real >,
		typename this_type::event::allocator::template rebind<
			std::pair<
				t_real const,
				typename this_type::event::action::point const* > >::other >
					dispatch_map;

	//-------------------------------------------------------------------------
	/** @param[in] i_package   �g�p����event-package�B
	    @param[in] i_allocator �������Ɏg��memory�����q�B
	 */
	public: explicit scene_stage(
		typename this_type::event::package::const_shared_ptr const&
			i_package,
		t_allocator const& i_allocator = t_allocator()):
	event_(i_package, i_allocator),
	packages_(typename this_type::package_map::key_compare(), i_allocator),
	screens_(typename this_type::screen_map::key_compare(), i_allocator),
	tokens_(typename this_type::token_map::key_compare(), i_allocator)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief �l�������B
	    @param[in,out] ��������ΏہB
	 */
	public: void swap(this_type& io_target)
	{
		this->event_.swap(io_target.event_);
		this->packages_.swap(io_target.packages_);
		this->screens_.swap(io_target.screens_);
		this->tokens_.swap(io_target.tokens_);
	}

	//-------------------------------------------------------------------------
	/** @brief �X�V�B
	    @param[in] i_fps   1�b�������frame���B
	    @param[in] i_count �i�߂�frame���B
	 */
	public: void update(t_real const i_fps, unsigned const i_count = 1)
	{
		typename psyq::scene_action< this_type >::update_parameters a_update;
		this->update(a_update, i_fps, i_count);
	}

	/** @brief �X�V�B
	    @param[in,out] io_update event-action��update�֐��ɓn�������B
	    @param[in]     i_fps     1�b�������frame���B
	    @param[in]     i_count   �i�߂�frame���B
	 */
	private: void update(
		typename psyq::scene_action< this_type >::update_parameters& io_update,
		t_real const   i_fps,
		unsigned const i_count = 1)
	{
		if (0 < i_fps)
		{
			// scene�̎��Ԃ��X�V�B
			this_type::event::line::scale::update_count(i_count);
			t_real const a_count(static_cast< t_real >(i_count));
			this_type::forward_scenes(this->tokens_, i_fps, a_count);

			// event���X�V�B
			typename this_type::dispatch_map a_points(
				typename this_type::dispatch_map::key_compare(),
				this->event_.lines_.get_allocator());
			this_type::forward_events(
				a_points, this->event_.lines_, i_fps, a_count);
			this->update_events(io_update, a_points);

			// scene���X�V�B
			this_type::update_scenes(this->tokens_);
		}
	}

	//-------------------------------------------------------------------------
	/** @brief scene-package���擾�B
	    package���ɑΉ�����scene-package�����݂��Ȃ��ꍇ�́A
	    file����ǂݍ���Œǉ�����B
	    @param[in] i_package �擾����scene-package�̖��Ohash�l�B
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

			// file��ǂݍ����scene-package���\�z�B
			this->make_package(i_package).swap(a_package);
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
		return this_type::event::pacakge::_find_shared_ptr(
			this->packages_, i_package);
	}

	/** @brief scene-package����菜���B
	    @param[in] i_package ��菜��scene-package�̖��Ohash�l�B
	    @return ��菜����scene-package�B��菜���Ȃ������ꍇ�͋�B
	 */
	public: psyq::scene_package::shared_ptr remove_package(
		typename t_hash::value const i_package)
	{
		return this_type::event::package::_remove_shared_ptr(
			this->packages_, i_package);
	}

	/** @brief file����scene-pacakge��ǂݍ��ށB
	    @param[in] i_package scene-package�̖��Ohash�l�B
	 */
	private: psyq::scene_package::shared_ptr make_package(
		typename t_hash::value const i_package)
	const
	{
		// event-package����package-path�������B
		typename this_type::package_path const* const a_path(
			this->event_.template
				find_package_value< typename this_type::package_path >(
					i_package));
		if (NULL != a_path)
		{
			// file����scene-package��ǂݍ��ށB
			psyq::scene_package::shared_ptr const a_package(
				psyq::scene_package::make(
					this->packages_.get_allocator(),
					this->event_.make_string(a_path->scene),
					this->event_.make_string(a_path->shader),
					this->event_.make_string(a_path->texture)));
			if (NULL != a_package.get())
			{
				return a_package;
			}
		}
		return psyq::_get_null_shared_ptr< psyq::scene_package >();
	}

	//-------------------------------------------------------------------------
	/** @brief screen��}���B
	    @param[in] i_name   �}������screen�̖��Ohash�l�B
		@param[in] i_screen �}������screen�B
	    @return �}������screen�B�}���Ɏ��s�����ꍇ�͋�B
	 */
	public: typename this_type::screen::shared_ptr const& insert_screen(
		typename t_hash::value const                  i_name,
		typename this_type::screen::shared_ptr const& i_screen)
	{
		return this_type::event::package::_insert_shared_ptr(
			this->screens_, i_name, i_screen);
	}

	/** @brief screen���擾�B
	    screen���ɑΉ�����screen�����݂��Ȃ��ꍇ�́A�V����screen�����B
	    @param[in] i_screen �擾����screen�̖��Ohash�l�B
	    @return screen���ɑΉ�����screen�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: typename this_type::screen::shared_ptr const& get_screen(
		typename t_hash::value const i_screen)
	{
		if (t_hash::EMPTY != i_screen)
		{
			// ������screen���猟���B
			typename this_type::screen::shared_ptr& a_screen(
				this->screens_[i_screen]);
			if (NULL != a_screen.get())
			{
				// screen�̎擾�ɐ����B
				return a_screen;
			}

			// �V����screen�����B
			PSYQ_ALLOCATE_SHARED< typename this_type::screen >(
				this->screens_.get_allocator(),
				this->screens_.get_allocator()).swap(a_screen);
			if (NULL != a_screen.get())
			{
				// screen�̎擾�ɐ����B
				return a_screen;
			}
			PSYQ_ASSERT(false);
			this->screens_.erase(i_screen);
		}

		// screen�̎擾�Ɏ��s�B
		return psyq::_get_null_shared_ptr< typename this_type::screen >();
	}

	/** @brief screen�������B
	    @param[in] i_screen ��������screen�̖��Ohash�l�B
	    @return ��������screen�B������Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::screen::shared_ptr const& find_screen(
		typename t_hash::value const i_screen)
	const
	{
		return this_type::event::package::_find_shared_ptr(
			this->screens_, i_screen);
	}

	/** @brief screen����菜���B
	    @param[in] i_screen ��菜��screen�̖��Ohash�l�B
	    @return ��菜����screen�B��菜���Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::screen::shared_ptr remove_screen(
		typename t_hash::value const i_screen)
	{
		return this_type::event::package::_remove_shared_ptr(
			this->screens_, i_screen);
	}

	//-------------------------------------------------------------------------
	/** @brief screen��token��}���B
	    @param[in] i_screen screen�̖��Ohash�l�B
	    @param[in] i_token  �}������token�̖��Ohash�l�B
	    @return �}������token�B���s�����ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr const& insert_screen_token(
		typename t_hash::value const i_screen,
		typename t_hash::value const i_token)
	{
		// token���擾���Ascreen�ɑ}���B
		typename this_type::screen* const a_screen(
			this->find_screen(i_screen).get());
		if (NULL != a_screen)
		{
			typename this_type::token::shared_ptr const& a_token(
				this->find_token(i_token));
			if (a_screen->insert_token(a_token))
			{
				return a_token;
			}
		}
		return psyq::_get_null_shared_ptr< typename this_type::token >();
	}

	/** @brief screen����token����菜���B
	    @param[in] i_screen screen�̖��Ohash�l�B
	    @param[in] i_token  ��菜��token�̖��Ohash�l�B
	    @return ��菜����token�B��菜���Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr const& remove_screen_token(
		typename t_hash::value const i_screen,
		typename t_hash::value const i_token)
	{
		// screen�������B
		typename this_type::screen* const a_screen(
			this->find_screen(i_screen).get());
		if (NULL != a_screen)
		{
			// token���������Ascreen�����菜���B
			typename this_type::token::shared_ptr const& a_token(
				this->find_token(i_token));
			if (a_screen->remove_token(a_token))
			{
				return a_token;
			}
		}
		return psyq::_get_null_shared_ptr< typename this_type::token >();
	}

	/** @brief ���ׂĂ�screen����token����菜���B
	    @param[in] i_token ��菜��token�̖��Ohash�l�B
	    @return ��菜����token�B��菜���Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr const& remove_screen_token(
		typename t_hash::value const i_token)
	{
		if (t_hash::EMPTY != i_token)
		{
			// token���擾�B
			typename this_type::token_map::iterator const a_token_pos(
				this->tokens_.find(i_token));
			if (this->tokens_.end() != a_token_pos)
			{
				// ���ׂĂ�screen����token����菜���B
				this->remove_screen_token(a_token_pos->second);
				return a_token_pos->second;
			}
		}
		return psyq::_get_null_shared_ptr< typename this_type::token >();
	}

	/** @brief ���ׂĂ�screen����token����菜���B
	    @param[in] i_token ��菜��token�B
	    @return ��菜����token�B��菜���Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr remove_screen_token(
		typename this_type::token::shared_ptr const& i_token)
	{
		// ���ׂĂ�screen����token����菜���B
		for (
			typename this_type::screen_map::const_iterator i =
				this->screens_.begin();
			this->screens_.end() != i;
			++i)
		{
			typename this_type::screen* const a_screen(
				i->second.get());
			if (NULL != a_screen)
			{
				a_screen->remove_token(i_token);
			}
		}
		return i_token;
	}

	//-------------------------------------------------------------------------
	/** @brief token��}���B
	    @param[in] i_name  �}������token�̖��Ohash�l�B
		@param[in] i_token �}������token�B
	    @return �}������token�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr const& insert_token(
		typename t_hash::value const                 i_name,
		typename this_type::token::shared_ptr const& i_token)
	{
		return this_type::event::package::_insert_shared_ptr(
			this->tokens_, i_name, i_token);
	}

	/** @brief token���擾�B
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

	/** @brief token�������B
	    @param[in] i_token ��������token�̖��Ohash�l�B
	    @return ������token�B������Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr const& find_token(
		typename t_hash::value const i_token)
	const
	{
		return this_type::event::package::_find_shared_ptr(
			this->tokens_, i_token);
	}

	/** @brief stage�ƑS�Ă�screen����token����菜���B
	    @param[in] i_token ��菜��token�̖��Ohash�l�B
	    @return ��菜����token�B��菜���Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr remove_token(
		typename t_hash::value const i_token)
	{
		typename this_type::token::shared_ptr a_token;
		if (t_hash::EMPTY != i_token)
		{
			// token���擾�B
			typename this_type::token_map::iterator const a_token_pos(
				this->tokens_.find(i_token));
			if (this->tokens_.end() != a_token_pos)
			{
				// stage����token����菜���B
				a_token.swap(a_token_pos->second);
				this->tokens_.erase(a_token_pos);

				// ���ׂĂ�screen����token����菜���B
				this->remove_screen_token(a_token);
			}
		}
		return a_token;
	}

	//-------------------------------------------------------------------------
	/** @brief time-scale����菜���B
	    @param[in] i_scale ��菜��time-scale�̖��Ohash�l�B
	    @return ��菜����time-scale�B��菜���Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::event::line::scale::shared_ptr const
		remove_scale(typename t_hash::value const i_scale)
	{
		typename this_type::event::line::scale::shared_ptr const a_scale(
			this->event_.remove_scale(i_scale));
		if (NULL != a_scale.get())
		{
			// token�W������time-scale����菜���B
			for (
				typename this_type::token_map::const_iterator i =
					this->tokens_.begin();
				i != this->tokens_.end();
				++i)
			{
				typename this_type::token* const a_token(i->second.get());
				if (NULL != a_token && a_scale == a_token->time_scale_)
				{
					a_token->time_scale_.reset();
				}
			}
		}
		return a_scale;
	}

	//-------------------------------------------------------------------------
	/** @brief scene�̎��Ԃ��X�V�B
	    @param[in] i_tokens scene-token�����B
	    @param[in] i_fps    1�b�������frame���B
	    @param[in] i_count  �i�߂�frame���B
	 */
	private: static void forward_scenes(
		typename this_type::token_map const& i_tokens,
		t_real const                         i_fps,
		t_real const                         i_count)
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
					this_type::event::line::scale::get_current(
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
	/** @brief event-line�̎��Ԃ��X�V���A��������event-point��container�ɓo�^�B
	    @param[in,out] io_points ��������event-point��o�^����container�B
	    @param[in]     i_lines   �X�V����event-line�̎����B
	    @param[in]     i_fps     1�b�������frame���B
	    @param[in]     i_count   �i�߂�frame���B
	 */
	private: static void forward_events(
		typename this_type::dispatch_map&          io_points,
		typename this_type::event::line_map const& i_lines,
		t_real const                               i_fps,
		t_real const                               i_count)
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

			// ��������event-point��container�ɓo�^�B
			a_line._dispatch(io_points);
		}
	}

	/** @brief container�ɓo�^����Ă���event�ɑΉ�����֐����Ăяo���B
	    @param[in] io_update event-action��update�֐��ɓn�������B
	    @param[in] i_points  ��������event-point���o�^����Ă���container�B
	 */
	private: void update_events(
		typename psyq::scene_action< this_type >::update_parameters& io_update,
		typename this_type::dispatch_map const&                      i_points)
	{
		for (
			typename this_type::dispatch_map::const_iterator i =
				i_points.begin();
			i_points.end() != i;
			++i)
		{
			// event-point���擾�B
			PSYQ_ASSERT(NULL != i->second);
			typename this_type::event::action::point const& a_point(
				*i->second);

			// event-point�ɑΉ�����event-action�������B
			typename this_type::event::action* const a_action(
				this_type::event::package::_find_shared_ptr(
					this->event_.actions_, a_point.type).get());

			// event�֐�object��K�p�B
			if (NULL != a_action)
			{
				io_update.reset(*this, a_point, i->first);
				a_action->update(io_update);
			}
		}
	}

	//-------------------------------------------------------------------------
	public: typename this_type::event       event_;    ///< event-stage�B
	public: typename this_type::package_map packages_; ///< scene-package�̎����B
	public: typename this_type::screen_map  screens_;  ///< scene-screen�̎����B
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
