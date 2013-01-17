#ifndef PSYQ_SCENE_RENDER_TARGET_HPP_
#define PSYQ_SCENE_RENDER_TARGET_HPP_

/// @cond
namespace psyq
{
	class render_target;
}
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief rendering��ƂȂ�buffer�B
 */
class psyq::render_target:
	private boost::noncopyable
{
	/// ����object�̌^�B
	public: typedef psyq::render_target this_type;

	//-------------------------------------------------------------------------
	/// rendering-buffer�̎�ʁB
	public: enum type
	{
		type_NONE,         ///< rendering���Ȃ��B
		type_FRAME_BUFFER, ///< frame-buffer��rendering����B
		type_TEXTURE,      ///< texture��redering����B
		type_CUBE_TEXTURE, ///< cube-texture��rendering����B
		type_POST_EFFECT,  ///< post-effect��rendering����B
	};

	//-------------------------------------------------------------------------
	/// flags_ �Ŏg��bit-mask�B
	private: enum flag
	{
		flag_TYPE    = 0x7, ///< type �̒l�����o���B
		flag_DRAWING = 0x8, ///< rendering���邩�B
	};

	//-------------------------------------------------------------------------
	public: render_target():
	//glare_(NULL),
	//tonemap_(NULL),
	//dof_(NULL),
	clear_color_(0, 0, 0, 1),
	clear_depth_(1),
	clear_stencil_(0),
	clear_buffer_(psyq_extern::surface_COLOR | psyq_extern::surface_DEPTH),
	flags_(this_type::type_FRAME_BUFFER)
	{
		this->texture_ = NULL;
	}

	public: ~render_target()
	{
		this->end_render();
		this->reset_post_effect();
		this->release_post_effect();
	}

	//-------------------------------------------------------------------------
	/** @brief rendering�̊J�n�����B
	    @return false�ȊO�Ȃ琬���Bfalse�Ȃ玸�s�B
	 */
	public: bool begin_render()
	{
		switch (this->get_render_type())
		{
			case this_type::type_FRAME_BUFFER:
			this->_clear_buffer();
			break;

			case this_type::type_TEXTURE:
			if (NULL == this->texture_)
			{
				PSYQ_ASSERT(false);
				return false;
			}
			psyq_extern::bind_texture(*this->texture_);
			this->_clear_buffer();
			break;

			case this_type::type_CUBE_TEXTURE:
			if (NULL == this->texture_)
			{
				PSYQ_ASSERT(false);
				return false;
			}
			break;

			case this_type::type_POST_EFFECT:
			if (NULL != this->post_effect_)
			{
				//psyq_extern::apply_post_effect(*this->post_effect_, i_camera, i_focus);

				psyq_extern::begin_post_effect(
					*this->post_effect_,
					this->clear_buffer_,
					this->clear_color_,
					this->clear_depth_,
					this->clear_stencil_);
			}
			break;

			default:
			return false;
		}

		this->flags_ |= this_type::flag_DRAWING;
		return true;
	}

	//-------------------------------------------------------------------------
	/** @brief rendering�̏I�������B
	    @return false�ȊO�Ȃ琬���Bfalse�Ȃ玸�s�B
	 */
	public: bool end_render()
	{
		switch (this->get_render_type())
		{
			case this_type::flag_DRAWING | this_type::type_FRAME_BUFFER:
			break;

			case this_type::flag_DRAWING | this_type::type_TEXTURE:
			case this_type::flag_DRAWING | this_type::type_CUBE_TEXTURE:
			psyq_extern::bind_frame_buffer();
			break;

			case this_type::flag_DRAWING | this_type::type_POST_EFFECT:
			if (NULL != this->post_effect_)
			{
				psyq_extern::end_post_effect(*this->post_effect_);
			}
			break;

			default:
			return false;
		}

		this->flags_ &= ~this_type::flag_DRAWING;
		return true;
	}

	//-------------------------------------------------------------------------
	/** @brief rendering-buffer�̎�ʂ��擾�B
	 */
	public: this_type::type get_type() const
	{
		return static_cast< this_type::type >(
			this_type::flag_TYPE & this->flags_);
	}

	/** @brief rendering���邩���擾�B
	    @return false�ȊO�Ȃ�Arendering����Bfalse�Ȃ�Arendering���Ȃ��B
	 */
	public: bool is_rendering() const
	{
		return 0 != (this->flags_ & this_type::flag_DRAWING);
	}

	//-------------------------------------------------------------------------
	/** @brief rendering-buffer��frame-buffer���ݒ肳��Ă��邩����B
	 */
	public: bool is_frame_buffer() const
	{
		return this_type::type_FRAME_BUFFER == this->get_type();
	}

	/** @brief frame-buffer��rendering-buffer�ɐݒ�B
	 */
	public: bool set_frame_buffer()
	{
		if (this->is_rendering())
		{
			return false;
		}

		this->release_post_effect();
		this->post_effect_ = NULL;
		this->set_render_type(this_type::type_FRAME_BUFFER);
		return true;
	}

	//-------------------------------------------------------------------------
	/** @brief rendering-buffer�ɐݒ肳��Ă���texture���擾�B
	 */
	public: psyq_extern::render_texture* get_texture()
	{
		return this_type::type_TEXTURE == this->get_type()?
			this->texture_: NULL;
	}

	/** @brief texture��rendering-buffer�ɐݒ�B
		@param[in] i_texture �ݒ肷��texture�B
	 */
	public: bool set_texture(psyq_extern::render_texture* const i_texture)
	{
		if (this->is_rendering() || NULL == i_texture)
		{
			return false;
		}

		this->release_post_effect();
		this->texture_ = i_texture;
		this->set_render_type(this_type::type_TEXTURE);
		return true;
	}

	//-------------------------------------------------------------------------
	/** @brief rendering-buffer�ɐݒ肳��Ă���cube-texture���擾�B
	 */
	public: psyq_extern::render_texture* get_cube_texture() const
	{
		return this_type::type_CUBE_TEXTURE == this->get_type()?
			this->texture_: NULL;
	}

	/** @brief cube-texture��rendering-buffer�ɐݒ�B
		@param[in] i_texture �ݒ肷��cube-texture�B
	 */
	public: bool set_cube_texture(psyq_extern::render_texture* const i_texture)
	{
		if (this->is_rendering() || NULL == i_texture)
		{
			return false;
		}

		this->release_post_effect();
		this->texture_ = i_texture;
		this->set_render_type(this_type::type_CUBE_TEXTURE);
		return true;
	}

	//-------------------------------------------------------------------------
	/** @brief rendering-buffer�ɐݒ肳��Ă���post-effect���擾�B
	 */
	public: psyq_extern::post_effect* get_post_effect() const
	{
		return this_type::type_POST_EFFECT == this->get_type()?
			this->post_effect_: NULL;
	}

	/** @brief post-effect��rendering-buffer�ɐݒ�B
	    @param[in,out] io_post_effect �ݒ肷��post-effect�B
	 */
	public: bool set_post_effect(psyq_extern::post_effect* const io_post_effect)
	{
		if (this->is_rendering())
		{
			return false;
		}

		this->release_post_effect();
		this->post_effect_ = io_post_effect;
		this->set_render_type(this_type::type_POST_EFFECT);
		if (NULL != io_post_effect)
		{
			psyq_extern::hold_post_effect(*io_post_effect);
		}
		return true;
	}

	//-------------------------------------------------------------------------
	/** @brief swap-buffer�ŁAcolor-buffer��clear����B
	    @param[in] i_color clear�Ɏg��color�l�B
	 */
	public: void set_clear_color(psyq_extern::math_vector4 const& i_color)
	{
		this->clear_buffer_ |= psyq_extern::surface_COLOR;
		this->clear_color_ = i_color;
	}

	/** @brief swap-buffer�ŁAcolor-buffer��clear���Ȃ��B
	 */
	public: void reset_clear_color()
	{
		this->clear_buffer_ &= ~psyq_extern::surface_COLOR;
	}

	//-------------------------------------------------------------------------
	/** @brief swap-buffer�ŁAdepth-buffer��clear����B
	    @param[in] i_depth clear�Ɏg��depth�l�B
	 */
	public: void set_clear_depth(float const i_depth)
	{
		this->clear_buffer_ |= psyq_extern::surface_DEPTH;
		this->clear_depth_ = i_depth;
	}

	/** @brief swap-buffer�ŁAdepth-buffer��clear���Ȃ��B
	 */
	public: void reset_clear_depth()
	{
		this->clear_buffer_ &= ~psyq_extern::surface_DEPTH;
	}

	//-------------------------------------------------------------------------
	/** @brief swap-buffer�ŁAstencil-buffer��clear����B
	    @param[in] i_stencil clear�Ɏg��stencil�l�B
	 */
	public: void set_clear_stencil(boost::uint32_t i_stencil)
	{
		this->clear_buffer_ |= psyq_extern::surface_STENCIL;
		this->clear_stencil_ = i_stencil;
	}

	/** @brief swap-buffer�ŁAstencil-buffer��clear���Ȃ��B
	 */
	public: void reset_clear_stencil()
	{
		this->clear_buffer_ &= ~psyq_extern::surface_STENCIL;
	}

	//-------------------------------------------------------------------------
	/** @brief user����͎g�p�֎~�Brendeing-buffer��clear����B
	 */
	public: void _clear_buffer() const
	{
		psyq_extern::clear_render_target(
			this->clear_buffer_,
			this->clear_color_,
			this->clear_depth_,
			this->clear_stencil_);
	}

	//-------------------------------------------------------------------------
	/** @brief rendering-buffer�̎�ʂ��擾�B
	 */
	private: unsigned get_render_type() const
	{
		return (this_type::flag_DRAWING | this_type::flag_TYPE) & this->flags_;
	}

	/** @brief rendering-buffer�̎�ʂ�ݒ�B
	    @param[in] i_type rendering-buffer�̎�ʁB
	 */
	private: void set_render_type(this_type::type const i_type)
	{
		this->flags_ = (~this_type::flag_TYPE & this->flags_) | i_type;
	}

	/** @brief �ێ����Ă���post-effect������B
	 */
	private: void release_post_effect()
	{
		if (NULL!= this->post_effect_ &&
			this_type::type_POST_EFFECT == this->get_type())
		{
			psyq_extern::release_post_effect(*this->post_effect_);
		}
	}

	/** @brief post-effect���������B
	 */
	private: void reset_post_effect()
	{
		/*
		this->reset_glare();
		this->reset_tonemap();
		this->reset_dof();
		this->reset_color_matrix();
		 */
	}

	//-------------------------------------------------------------------------
	private: union
	{
		/// rendering-buffer�Ɏg���Ă���texture�B
		psyq_extern::render_texture* texture_;

		/// rendering-buffer�Ɏg���Ă���post-effect�B
		psyq_extern::post_effect* post_effect_;
	};
	//private: pfx_glare*                glare_;
	//private: pfx_tonemap*              tonemap_;
	//private: pfx_dof*                  dof_;
	//private: this_type::color_matrices color_matrices_;
	private: psyq_extern::math_vector4  clear_color_;   ///< clear�Ɏg��color�l�B
	private: float                      clear_depth_;   ///< clear�Ɏg��depth�l�B
	private: boost::uint32_t            clear_stencil_; ///< clear�Ɏg��stencil�l�B
	private: boost::uint32_t            clear_buffer_;  ///< clear����buffer�B
	private: boost::uint32_t            flags_;         ///< �e��flag��ێ�����B
};

#endif // !PSYQ_SCENE_RENDER_TARGET_HPP_
