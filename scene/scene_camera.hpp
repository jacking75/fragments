#ifndef PSYQ_SCENE_CAMERA_HPP_
#define PSYQ_SCENE_CAMERA_HPP_

namespace psyq
{
	template< typename, typename > class scene_camera;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_hash, typename t_real >
class psyq::scene_camera:
	private boost::noncopyable
{
	typedef psyq::scene_camera< t_hash, t_real > this_type;

	//-------------------------------------------------------------------------
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash;
	public: typedef t_real real;
	public: typedef psyq::scene_token< t_hash, t_real > token;

	//-------------------------------------------------------------------------
	public: scene_camera():
	camera_node_(NULL),
	focus_node_(NULL)
	{
		// pass
	}

	public: explicit scene_camera(
		typename this_type::token::shared_ptr const& i_token,
		char const* const                            i_name = NULL)
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
		char const* const                            i_camera_name,
		typename this_type::token::shared_ptr const& i_focus_token,
		char const* const                            i_focus_name)
	{
		this->set_node(i_camera_token, i_camera_name);
		this->set_focus_node(i_focus_token, i_focus_name);
	}

	//-------------------------------------------------------------------------
	/** @brief camera��ݒ�B
	    @param[in] i_token camera-node������scene-token�B
	    @param[in] i_name  camera�Ƃ��Ďg��node��ID������B
	 */
	public: psyq_extern::scene_node const* set_node(
		typename this_type::token::shared_ptr const& i_token,
		char const* const                            i_name = NULL,
		char const* const                            i_focus = NULL)
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
				this->set_focus_node(i_token, i_focus);
				return a_node;
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
		char const* const                    i_name)
	{
		typename this_type::token* const a_token(i_token.get());
		if (NULL != i_name && NULL != a_token)
		{
			psyq_extern::scene_node* const a_node(
				psyq_extern::find_node(a_token->scene_, i_name));
			if (NULL != a_node)
			{
				this->focus_token_ = i_token;
				this->focus_node_ = a_node;
				return a_node;
			}
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

	//-------------------------------------------------------------------------
	private: typename this_type::token::shared_ptr camera_token_;
	private: psyq_extern::scene_node const*        camera_node_;
	private: typename this_type::token::shared_ptr focus_token_;
	private: psyq_extern::scene_node const*        focus_node_;
};

#endif // PSYQ_SCENE_CAMERA_HPP_
