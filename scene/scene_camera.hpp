#ifndef PSYQ_SCENE_CAMERA_HPP_
#define PSYQ_SCENE_CAMERA_HPP_

namespace psyq
{
	template< typename, typename, typename, typename > class scene_camera;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief scene-stage�ɔz�u����camera�B
    @tparam t_hash      event-package�Ŏg���Ă���hash�֐��B
    @tparam t_real      event-package�Ŏg���Ă�������̌^�B
    @tparam t_name      scene-node�̖��O�̌^�B
    @tparam t_allocator �g�p����memory�����q�̌^�B
 */
template<
	typename t_hash,
	typename t_real,
	typename t_name,
	typename t_allocator >
class psyq::scene_camera:
	private boost::noncopyable
{
	public: typedef psyq::scene_camera< t_hash, t_real, t_name, t_allocator >
		this_type;

	//-------------------------------------------------------------------------
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash;
	public: typedef t_real real;
	public: typedef t_name name;
	public: typedef t_allocator allocator;
	public: typedef psyq::scene_token< t_hash, t_real > token;

	//-------------------------------------------------------------------------
	private: typedef std::vector<
		typename this_type::token::shared_ptr,
		typename t_allocator::template rebind<
			typename this_type::token::shared_ptr >::other >
				token_container;

	//-------------------------------------------------------------------------
	public: explicit scene_camera(t_allocator const& i_allocator):
	tokens_(i_allocator),
	camera_node_(NULL),
	focus_node_(NULL)
	{
		// pass
	}

	public: explicit scene_camera(
		typename this_type::token::shared_ptr const& i_token,
		t_name const                                 i_name)
	{
		this->set_node(i_token, i_name);
	}

	/** @param[in] i_camera_token camera�Ƃ��Ďg��scene-token�B
	    @param[in] i_camera_name  camera�Ƃ��Ďg��node��ID������B
		@param[in] i_focus_token  �œ_�Ƃ��Ďg��scene-token�B
	    @param[in] i_focus_name   �œ_�Ƃ��Ďg��node��ID������B
	 */
	public: scene_camera(
		typename this_type::token::shared_ptr const& i_camera_token,
		t_name const                                 i_camera_name,
		typename this_type::token::shared_ptr const& i_focus_token,
		t_name const                                 i_focus_name)
	{
		this->set_node(
			i_camera_token, i_camera_name, i_focus_token, i_focus_name);
	}

	//-------------------------------------------------------------------------
	/** @brief camera��ݒ�B
	    @param[in] i_token camera-node������scene-token�B
	    @param[in] i_name  camera�Ƃ��Ďg��node��ID���B
	 */
	public: psyq_extern::scene_node const* set_node(
		typename this_type::token::shared_ptr const& i_token,
		t_name const                                 i_name)
	{
		typename this_type::token* const a_token(i_token.get());
		if (NULL != a_token)
		{
			psyq_extern::scene_node* const a_node(
				psyq_extern::find_camera_node(a_token->scene_, i_name));
			if (NULL != a_node)
			{
				this->camera_token_ = i_token;
				this->camera_node_ = a_node;
				return a_node;
			}
		}
		return NULL;
	}

	/** @brief camera�Əœ_��ݒ�B
	    @param[in] i_token  camera-node������scene-token�B
	    @param[in] i_camera camera�Ƃ��Ďg��node��ID���B
	    @param[in] i_focus  �œ_�Ƃ��Ďg��node��ID���B
	 */
	public: psyq_extern::scene_node const* set_node(
		typename this_type::token::shared_ptr const& i_token,
		t_name const                                 i_camera,
		t_name const                                 i_focus)
	{
		return this->set_node(i_token, i_camera, i_token, i_focus);
	}

	/** @brief camera�Əœ_��ݒ�B
	    @param[in] i_camera_token camera-node������scene-token�B
	    @param[in] i_camera_name  camera�Ƃ��Ďg��node��ID���B
	    @param[in] i_focus_token  �œ_node������scene-token�B
	    @param[in] i_focus_name   �œ_�Ƃ��Ďg��node��ID���B
	 */
	public: psyq_extern::scene_node const* set_node(
		typename this_type::token::shared_ptr const& i_camera_token,
		t_name const                                 i_camera_name,
		typename this_type::token::shared_ptr const& i_focus_token,
		t_name const                                 i_focus_name)
	{
		psyq_extern::scene_node* const a_focus_node(
			this_type::find_focus_node(i_focus_token, i_focus_name));
		if (NULL != a_focus_node)
		{
			psyq_extern::scene_node* const a_camera_node(
				this->set_node(i_camera_token, i_camera_name));
			if (NULL != a_camera_node)
			{
				this->focus_token_ = i_focus_token;
				this->focus_node_ = a_focus_node;
				return a_camera_node;
			}
		}
		return NULL;
	}

	/** @brief camera-node���擾�B
	 */
	public: psyq_extern::scene_node const* get_node() const
	{
		return this->camera_node_;
	}

	/** @brief camera-node������scene-token���擾�B
	 */
	public: typename this_type::token::shared_ptr const& get_token() const
	{
		return this->camera_token_;
	}

	//-------------------------------------------------------------------------
	/** @brief �œ_��ݒ�B
	    @param[in] i_token �œ_node������scene-token�B
	    @param[in] i_name  �œ_�Ƃ��Ďg��node��ID������B
	 */
	public: psyq_extern::scene_node const* set_focus_node(
		typename this_type::token::shared_ptr const& i_token,
		t_name const                                 i_name)
	{
		psyq_extern::scene_node* const a_node(
			this_type::find_focus_node(i_token, i_name));
		if (NULL != a_node)
		{
			this->focus_token_ = i_token;
			this->focus_node_ = a_node;
			return a_node;
		}
		return NULL;
	}

	/** @brief �œ_node���擾�B
	 */
	public: psyq_extern::scene_node const* get_focus_node()
	{
		return this->focus_node_;
	}

	/** @brief �œ_node������scene-token���擾�B
	 */
	public: typename this_type::token::shared_ptr const& get_focus_token() const
	{
		return this->focus_token_;
	}

	/** @brief �œ_�������擾�B
	 */
	public: float get_focus_distance() const
	{
		return NULL != this->camera_node_ && NULL != this->focus_node_?
			psyq_extern::distance(*this->camera_node_, *this->focus_node_): 0;
	}

	private: static psyq_extern::scene_node const* find_focus_node(
		typename this_type::token::shared_ptr const& i_token,
		t_name const                                 i_name)
	{
		typename this_type::token* const a_token(i_token.get());
		return NULL != i_name && NULL != a_token?
			psyq_extern::find_node(a_token->scene_, i_name): NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief �`��scene-token��ǉ��B
	    @param[in] i_token �ǉ�����scene-token�B
	 */
	public: bool insert_token(
		typename this_type::token::shared_ptr const& i_token)
	{
		if (NULL == i_token.get())
		{
			return false;
		}
		if (!this->find_token(i_token))
		{
			this->tokens_.push_back(i_token);
		}
		return true;
	}

	/** @brief �`��scene-tokens�������B
	    @param[in] i_token ��������scene-token�B
	    @return true�Ȃ猩�������Bfalse�Ȃ猩����Ȃ������B
	 */
	public: bool find_token(
		typename this_type::token::shared_ptr const& i_token)
	const
	{
		return this->find_token_index(i_token.get()) < this->tokens_.size();
	}

	/** @brief �`��scene-token���폜�B
	    @param[in] i_token �폜����scene-token�B
	 */
	public: bool erase_token(
		typename this_type::token::shared_ptr const& i_token)
	{
		std::size_t const a_index(this->find_token_index(i_token.get()));
		if (a_index < this->tokens_.size())
		{
			this->tokens_.at(a_index).swap(this->tokens_.back());
			this->tokens_.pop_back();
			return true;
		}
		return false;
	}

	//-------------------------------------------------------------------------
	/** @brief render-target�ɕ`�悷��B
	    @param[in] i_target �`���render-target�B
	    @param[in] i_camera
	        �`��Ɏg��camera�BNULL�̏ꍇ��set_camera()�Ŏw�肳�ꂽcamera���g���B
	    @param[in] i_light
	        �`��Ɏg��light�BNULL�̏ꍇ��set_light()�Ŏw�肳�ꂽlight���g���B
	 */
	public: void draw(
		psyq::render_target const&           i_target,
		psyq_extern::scene_node const* const i_camera = NULL,
		psyq_extern::scene_unit const* const i_light = NULL)
	{
		PSYQ_ASSERT(i_target.is_drawing());

		// camera��ݒ�B
		psyq_extern::scene_node const* const a_camera(
			psyq_extern::set_camera(i_camera, this->camera_.get()));

		// light��ݒ�B
		psyq_extern::set_light(i_light, this->light_.get());

		// �`��B
		if (NULL != a_camera)
		{
			psyq_extern::draw_tokens(this->tokens_, i_target);
		}
	}

	//-------------------------------------------------------------------------
	private: std::size_t find_token_index(
		typename this_type::token const* const i_token)
	const
	{
		if (NULL != i_token)
		{
			for (std::size_t i = 0; i < this->tokens_.size(); ++i)
			{
				if (this->tokens_.at(i).get() == i_token)
				{
					return i;
				}
			}
		}
		return (std::numeric_limits< std::size_t >::max)();
	}

	//-------------------------------------------------------------------------
	public:  typename this_type::token::shared_ptr light_;
	private: typename this_type::token_container   tokens_;
	private: typename this_type::token::shared_ptr camera_token_;
	private: psyq_extern::scene_node const*        camera_node_;
	private: typename this_type::token::shared_ptr focus_token_;
	private: psyq_extern::scene_node const*        focus_node_;
};

#endif // PSYQ_SCENE_CAMERA_HPP_
