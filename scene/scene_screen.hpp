#ifndef PSYQ_SCENE_SCREEN_HPP_
#define PSYQ_SCENE_SCREEN_HPP_

//#include <psyq/scene/scene_token.hpp>

/// @cond
namespace psyq
{
	template< typename, typename, typename, typename > class scene_screen;
}
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief scene��`�悷��screen�B
    @tparam t_hash      @copydoc scene_screen::hash
    @tparam t_real      @copydoc scene_screen::real
    @tparam t_name      @copydoc scene_screen::name
    @tparam t_allocator @copydoc scene_screen::allocator
 */
template<
	typename t_hash,
	typename t_real,
	typename t_name,
	typename t_allocator >
class psyq::scene_screen:
	private boost::noncopyable
{
	/// ����object�̌^�B
	public: typedef psyq::scene_screen< t_hash, t_real, t_name, t_allocator >
		this_type;

	//-------------------------------------------------------------------------
	/// ����instance�̕ێ��q�B
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;

	/// ����instance�̊Ď��q�B
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;

	//-------------------------------------------------------------------------
	/// event-package�Ŏg���Ă���hash�֐��̌^�B
	public: typedef t_hash hash;

	/// event-package�Ŏg���Ă�������̌^�B
	public: typedef t_real real;

	/// scene-node�̎��ʖ��̌^�B
	public: typedef t_name name;

	/// �g�p����memory�����q�̌^�B
	public: typedef t_allocator allocator;

	/// ����instance�Ŏg�p���� scene_token �̌^�B
	public: typedef psyq::scene_token< t_hash, t_real > token;

	//-------------------------------------------------------------------------
	/// this_type::token �̔z��^�B
	private: typedef std::vector<
		typename this_type::token::shared_ptr,
		typename t_allocator::template rebind<
			typename this_type::token::shared_ptr >::other >
				token_container;

	//-------------------------------------------------------------------------
	/** @param[in] i_allocator �������Ɏg��memory�����q�B
	 */
	public: explicit scene_screen(
		t_allocator const& i_allocator = t_allocator()):
	tokens_(i_allocator),
	camera_node_(NULL),
	focus_node_(NULL)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief �l�������B
	    @param[in,out] io_target �l����������instance�B
	 */
	public: void swap(this_type& io_target)
	{
		this->light_.swap(io_target.light_);
		this->tokens_.swap(io_target.tokens_);
		this->camera_token_.swap(io_target.camera_token_);
		std::swap(this->camera_node_, io_target.camera_node_);
		this->focus_token_.swap(io_target.focus_token_);
		std::swap(this->focus_node_, io_target.focus_node_);
	}

	//-------------------------------------------------------------------------
	/** @brief render-target�ɕ`�悷��B
	    @param[in] i_target �`���render-target�B
	    @param[in] i_camera
	        �`��Ɏg��camera�BNULL�̏ꍇ�͐ݒ肳��Ă���camera���g���B
	    @param[in] i_light
	        �`��Ɏg��light�BNULL�̏ꍇ�͐ݒ肳��Ă���light���g���B
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
	/** @brief �`�悷��scene-token��ǉ��B
	    @param[in] i_token �ǉ�����scene-token�B
		@retval !=false �����B
	    @retval ==false ���s�B�ǉ�����token���󂾂����B
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

	/** @brief �`�悷��scene-tokens�������B
	    @param[in] i_token ��������scene-token�B
	    @retval !=false token���������B
	    @retval ==false token��������Ȃ������B
	 */
	public: bool find_token(
		typename this_type::token::shared_ptr const& i_token)
	const
	{
		return this->find_token_index(i_token.get()) < this->tokens_.size();
	}

	/** @brief �`�悷��scene-token����菜���B
	    @param[in] i_token ��菜��scene-token�B
	    @retval !=false �����B
	    @retval ==false ���s�B��菜��token��������Ȃ������B
	 */
	public: bool remove_token(
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

	/** @brief �`�悷��scene-token�����ׂĎ�菜���B
	 */
	public: void remove_tokens()
	{
		typename this_type::token_container().swap(this->tokens_);
	}

	//-------------------------------------------------------------------------
	/** @brief camera-node���擾�B
	    @retval !=NULL camera-node�ւ�pointer�B
	    @retval ==NULL camera-node���ݒ肳��Ă��Ȃ��B
	 */
	public: psyq_extern::scene_node const* get_camera_node() const
	{
		return this->camera_node_;
	}

	/** @brief camera-node������scene-token���擾�B
	    @return
	        camera-node������scene-token���w��smart-pointer�B
	        �������A�œ_node���ݒ肳��ĂȂ��ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr const& get_camera_token()
	const
	{
		return this->camera_token_;
	}

	/** @brief camera��ݒ�B
	    @param[in] i_token camera-node������scene-token�B
	    @param[in] i_name  camera-node�̎��ʖ��B
	    @retval !=NULL �ݒ肵��camera-node�ւ�pointer�B
	    @retval ==NULL �ݒ�Ɏ��s�B
	 */
	public: psyq_extern::scene_node const* set_camera(
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
	    @param[in] i_token  camera-node�Əœ_node������scene-token�B
	    @param[in] i_camera camera-node�̎��ʖ��B
	    @param[in] i_focus  �œ_node�̎��ʖ��B
	    @retval !=NULL �ݒ肵��camera-node�ւ�pointer�B
	    @retval ==NULL �ݒ�Ɏ��s�B
	 */
	public: psyq_extern::scene_node const* set_camera(
		typename this_type::token::shared_ptr const& i_token,
		t_name const                                 i_camera,
		t_name const                                 i_focus)
	{
		return this->set_node(i_token, i_camera, i_token, i_focus);
	}

	/** @brief camera����菜���B
	 */
	public: void remove_camera()
	{
		this->camera_token_.reset();
		this->camera_node_ = NULL;
	}

	/** @brief camera�Əœ_��ݒ�B
	    @param[in] i_camera_token camera-node������scene-token�B
	    @param[in] i_camera_name  camera-node�̎��ʖ��B
	    @param[in] i_focus_token  �œ_node������scene-token�B
	    @param[in] i_focus_name   �œ_node�̎��ʖ��B
	    @retval !=NULL �ݒ肵��camera-node�ւ�pointer�B
	    @retval ==NULL �ݒ�Ɏ��s�B
	 */
	public: psyq_extern::scene_node const* set_camera(
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

	//-------------------------------------------------------------------------
	/** @brief �œ_node���擾�B
	    @retval !=NULL �œ_node�ւ�pointer�B
	    @retval ==NULL �œ_node���ݒ肳��Ă��Ȃ��B
	 */
	public: psyq_extern::scene_node const* get_focus_node()
	{
		return this->focus_node_;
	}

	/** @brief �œ_node������scene-token���擾�B
	    @return
	        �œ_node������scene-token���w��smart-pointer�B
	        �������A�œ_node���ݒ肳��ĂȂ��ꍇ�͋�B
	 */
	public: typename this_type::token::shared_ptr const& get_focus_token()
	const
	{
		return this->focus_token_;
	}

	/** @brief �œ_�������擾�B
	    @retval camera-node����œ_node�܂ł̋����B
	 */
	public: float get_focus_distance() const
	{
		return NULL != this->camera_node_ && NULL != this->focus_node_?
			psyq_extern::distance(*this->camera_node_, *this->focus_node_): 0;
	}

	/** @brief �œ_����菜���B
	 */
	public: void remove_focus()
	{
		this->focus_token_.reset();
		this->focus_node_ = NULL;
	}

	/** @brief �œ_��ݒ�B
	    @param[in] i_token �œ_node������scene-token�B
	    @param[in] i_name  �œ_node�̎��ʖ��B
	    @retval !=NULL �ݒ肵���œ_node�ւ�pointer�B
	    @retval ==NULL �ݒ�Ɏ��s�B
	 */
	public: psyq_extern::scene_node const* set_focus(
		typename this_type::token::shared_ptr const& i_token,
		t_name const                                 i_name)
	{
		psyq_extern::scene_node const* const a_node(
			this_type::find_focus_node(i_token, i_name));
		if (NULL != a_node)
		{
			this->focus_token_ = i_token;
			this->focus_node_ = a_node;
			return a_node;
		}
		return NULL;
	}

	//-------------------------------------------------------------------------
	private: std::size_t find_token_index(
		typename this_type::token const* const i_token)
	const
	{
		if (NULL != i_token)
		{
			std::size_t a_rest(this->tokens_.size());
			while (0 < a_rest)
			{
				--a_rest;
				if (this->tokens_.at(a_rest).get() == i_token)
				{
					return a_rest;
				}
			}
		}
		return (std::numeric_limits< std::size_t >::max)();
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
	public:  typename this_type::token::shared_ptr light_;
	private: typename this_type::token_container   tokens_;
	private: typename this_type::token::shared_ptr camera_token_;
	private: psyq_extern::scene_node const*        camera_node_;
	private: typename this_type::token::shared_ptr focus_token_;
	private: psyq_extern::scene_node const*        focus_node_;
};

//-----------------------------------------------------------------------------
namespace std
{
	template<
		typename t_hash,
		typename t_real,
		typename t_name,
		typename t_allocator >
	void swap(
		psyq::scene_screen< t_hash, t_real, t_name, t_allocator >& io_left,
		psyq::scene_screen< t_hash, t_real, t_name, t_allocator >& io_right)
	{
		io_left.swap(io_right);
	}
};

#endif // PSYQ_SCENE_SCREEN_HPP_
