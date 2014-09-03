﻿#ifndef PSYQ_GEOMETRY_MOSP_NUT_HPP_
#define PSYQ_GEOMETRY_MOSP_NUT_HPP_

//#include "psyq/geometry/shape.hpp"

namespace psyq
{
    namespace geometry
    {
        /// モートン順序を用いた空間分割木で、衝突判定を行う。
        namespace mosp
        {
            /// @cond
            template<typename = PSYQ_MOSP_SPACE_DEFAULT> class nut;
            /// @endcond
        } // namespace mosp
    } // namespace geometry
} // namespace psyq

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief  mosp_tree 空間分割木に取りつける、衝突判定オブジェクトの基底型。
    @tparam template_space @copydoc psyq::mosp_tree::space
 */
template<typename template_space>
class psyq::geometry::mosp::nut
{
    /// thisが指す値の型。
    private: typedef nut this_type;

    //-------------------------------------------------------------------------
    /// nut で使うモートン空間の型。
    public: typedef template_space space;
    /// 空間分割木に取りつける mosp_node 。
    public: typedef psyq::mosp_node<this_type*, typename this_type::space::order>
        node;

    public: template<typename template_shape> class concrete;
    /// mosp_tree に取付可能な、球の衝突判定オブジェクト。
    public: typedef concrete<
        psyq::geometry::ball<typename this_type::space::coordinate>>
            ball;
    /// mosp_tree に取付可能な、線分の衝突判定オブジェクト。
    public: typedef concrete<
        psyq::geometry::segment<typename this_type::space::coordinate>>
            segment;
    /// mosp_tree に取付可能な、半直線の衝突判定オブジェクト。
    public: typedef concrete<
        psyq::geometry::ray<typename this_type::space::coordinate>>
            ray;
    /// mosp_tree に取付可能な、直方体の衝突判定オブジェクト。
    public: typedef concrete<
        psyq::geometry::box<typename this_type::space::coordinate>>
            box;

    //-------------------------------------------------------------------------
    protected: nut():
        node_(this),
        aabb_(
            typename this_type::space::coordinate::aabb(
                this_type::space::coordinate::make(0),
                this_type::space::coordinate::make(0)))
    {}

    /// *thisを mosp_tree から取り外す。
    public: virtual ~nut() {}

    //-------------------------------------------------------------------------
    /** @brief *thisを mosp_tree 空間分割木に取りつける。

        現在取りつけられている mosp_tree から*thisを切り離し、
        新しい mosp_tree に*thisを取りつける。

        @param[in,out] io_tree *thisを取りつける mosp_tree 。
        @sa detach_tree() is_attached()
     */
    public: template<typename template_tree>
    void attach_tree(template_tree& io_tree)
    {
        // AABBを更新してから取りつける。
        if (!this->node_.is_attached())
        {
            this->update_aabb();
        }
        this->node_.attach_tree(io_tree, this->get_aabb());
    }

    /// @copydoc this_type::node::detach_tree()
    public: void detach_tree()
    {
        this->node_.detach_tree();
    }

    /// @copydoc this_type::node::is_attached()
    protected: bool is_attached() const PSYQ_NOEXCEPT
    {
        return this->node_.is_attached();
    }

    /** @brief thisが持つAABBを取得する。
     */
    public: typename this_type::space::coordinate::aabb const& get_aabb()
    const PSYQ_NOEXCEPT
    {
        return this->aabb_;
    }

    /** @brief thisが持つAABBを更新する。
     */
    protected: virtual void update_aabb() = 0;

    //-------------------------------------------------------------------------
    /// 衝突判定オブジェクトに対応する分割空間ノード。
    private: typename this_type::node node_;
    /// 衝突判定オブジェクトの絶対座標系AABB。
    protected: typename this_type::space::coordinate::aabb aabb_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief  mosp_tree 空間分割木に取りつける、衝突判定オブジェクト。
    @tparam template_space @copydoc psyq::mosp_tree::space
    @tparam template_shape @copydoc psyq::geometry::mosp::nut::shape
 */
template<typename template_space>
template<typename template_shape>
class psyq::geometry::mosp::nut<template_space>::concrete:
    public psyq::geometry::mosp::nut<template_space>
{
    /// thisが指す値の型。
    private: typedef concrete this_type;
    /// this_type の基底型。
    public: typedef psyq::geometry::mosp::nut<template_space> base_type;

    /// 衝突判定オブジェクトの幾何形状。
    public: typedef template_shape shape;

    //-------------------------------------------------------------------------
    /** @brief 衝突判定に使う形状を構築する。
        @param[in] in_shape 衝突判定に使う形状の初期値。
     */
    public: explicit concrete(typename this_type::shape in_shape):
        shape_(std::move(in_shape))
    {}

    //-------------------------------------------------------------------------
    /** @brief 衝突判定に使う形状を取得する。
        @return 衝突判定に使う形状。
     */
    public: typename this_type::shape const& get_shape() const PSYQ_NOEXCEPT
    {
        return this->shape_;
    }

    /** @brief *thisを mosp_tree から取り外し、衝突判定に使う形状を取得する。

        衝突判定に使う幾何形状を更新したい場合は、
        この関数の戻り値が参照する幾何形状を書き換えた後、
        base_type::attach_tree() で mosp_tree に取りつける。

        @return 衝突判定に使う形状。
     */
    public: typename this_type::shape& get_mutable_shape()
    {
        if (this->is_attached())
        {
            this->detach_tree();
        }
        return this->shape_;
    }

    //-------------------------------------------------------------------------
    protected: void update_aabb() override
    {
        this->aabb_ = psyq::geometry::make_aabb(this->get_shape());
    };

    //-------------------------------------------------------------------------
    /// 衝突判定オブジェクトの形状。
    protected: typename this_type::shape shape_;
};


//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
namespace psyq
{
    namespace test
    {
        inline void mosp_collision()
        {
            typedef PSYQ_MOSP_SPACE_DEFAULT psyq_mosp_space;
            typedef psyq::mosp_collision::nut<psyq_mosp_space> psyq_mosp_nut;
            typedef psyq::mosp_tree<psyq_mosp_nut*> psyq_mosp_tree;
            psyq_mosp_tree::node_map::allocator_type::arena::shared_ptr
                local_mosp_arena(
                    new psyq_mosp_tree::node_map::allocator_type::arena(16));
            psyq_mosp_tree local_mosp_tree(
                psyq_mosp_space::coordinate::aabb(
                    psyq_mosp_space::coordinate::vector(-65536, -65536, -65536),
                    psyq_mosp_space::coordinate::vector( 65536,  65536,  65536)),
                1024,
                psyq_mosp_tree::allocator_type(local_mosp_arena));
            psyq_mosp_nut::ball local_mosp_ball(
                psyq_mosp_nut::ball::shape(
                    psyq_mosp_space::coordinate::vector(2, 3, 4), 1));
            local_mosp_ball.attach_tree(local_mosp_tree);
            psyq_mosp_nut::ray local_mosp_ray(
                psyq_mosp_nut::ray::shape(
                    psyq_mosp_space::coordinate::vector(1, 2, 3),
                    psyq_mosp_space::coordinate::vector(4, 5, 6)));
            local_mosp_ray.attach_tree(local_mosp_tree);
            local_mosp_tree.detect_collision_batch(
                [](
                    psyq_mosp_nut* const in_nut_0,
                    psyq_mosp_nut* const in_nut_1)
                {
                    PSYQ_ASSERT(in_nut_0 != nullptr);
                    PSYQ_ASSERT(in_nut_1 != nullptr);
                    PSYQ_ASSERT(in_nut_0 != in_nut_1);

                    // AABBが衝突しているか判定する。
                    bool const local_aabb_collision(
                        psyq_mosp_space::coordinate::aabb::detect_collision(
                            in_nut_0->get_aabb(), in_nut_1->get_aabb()));
                    if (local_aabb_collision)
                    {
                    }
                });
            local_mosp_ray.attach_tree(local_mosp_tree);
        }
    }
}

#endif // !defined(PSYQ_GEOMETRY_MOSP_NUT_HPP_)
