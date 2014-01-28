﻿#ifndef PSYQ_MOSP_TREE_OLD_HPP_
#define PSYQ_MOSP_TREE_OLD_HPP_

namespace psyq
{
    /// @cond
    namespace internal
    {
        template<typename> class mosp_node;
    }
    namespace old
    {
        template<typename> class mosp_handle;
        template<typename, typename = PSYQ_MOSP_SPACE_DEFAULT> class mosp_tree;
    }
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief morton順序による空間分割木を用いた衝突判定のhandle。

    使い方の概要。
    -# 衝突する物体の識別値を、 mosp_handle::object_ に代入しておく。
    -# mosp_handle::attach_tree() を呼び出し、
       mosp_handle を空間分割木に取りつける。
    -# mosp_tree::detect_collision() を呼び出し、衝突を判定する。

    @tparam template_collision_object @copydoc mosp_handle::collision_object
 */
template<typename template_collision_object>
class psyq::old::mosp_handle
{
    /// *thisの型。
    private: typedef mosp_handle<template_collision_object> self;

    /// 衝突判定オブジェクトの識別子。
    public: typedef template_collision_object collision_object;

    //-------------------------------------------------------------------------
    /** @brief 衝突判定のhandleを構築する。
        @param[in] in_object thisに対応する、衝突判定オブジェクトの識別子。
     */
    public: explicit mosp_handle(template_collision_object in_object):
        node_(nullptr),
        object_(std::move(in_object))
    {}

    /// copy-constructorは使用禁止。
    private: mosp_handle(self const&);

    /** @brief move-constructor。
        @param[in,out] io_source 移動元となるinstance。
     */
    public: mosp_handle(self&& io_source):
        node_(io_source.node_),
        object_(std::move(io_source.object_))
    {
        if (io_source.node_ != nullptr)
        {
            io_source.node_ = nullptr;
            this->node_->handle_ = this;
        }
    }

    /// @brief 空間分割木からthisを取り外す。
    public: ~mosp_handle()
    {
        this->detach_tree();
    }

    /// copy代入演算子は使用禁止。
    private: self& operator=(const self&);

    /** @brief move代入演算子。
        @param[in,out] io_source 移動元となるinstance。
     */
    public: self& operator=(self&& io_source)
    {
        if (this != &io_source)
        {
            this->~self();
            new(this) self(std::move(io_source));
        }
        return *this;
    }

    //-------------------------------------------------------------------------
    /** @brief *thisを mosp_tree 空間分割木に取りつける。

        現在取りつけられている mosp_tree から*thisを切り離し、
        新しい mosp_tree に*thisを取りつける。

        @param[in,out] io_tree *thisを取りつける mosp_tree 。
        @param[in]     in_aabb *thisに対応する衝突領域の、絶対座標系AABB。
        @sa detach_tree() is_attached()
     */
    public: template<typename template_mosp_tree>
    bool attach_tree(
        template_mosp_tree&                                          io_tree,
        typename template_mosp_tree::space::coordinates::aabb const& in_aabb)
    {
        // 新しいnodeを用意する。
        auto const local_node(io_tree.make_node(in_aabb));
        if (local_node == nullptr)
        {
            return false;
        }
        local_node->handle_ = this;

        // 古いnodeから新しいnodeへ切り替える。
        if (this->node_ != nullptr)
        {
            PSYQ_ASSERT(this == this->node_->get_handle());
            this->node_->handle_ = nullptr;
        }
        this->node_ = local_node;
        return true;
    }

    /** @brief *thisを mosp_tree 空間分割木から取り外す。

        現在取りつけられている mosp_tree から、*thisを切り離す。

        @sa attach_tree() is_attached()
     */
    public: void detach_tree()
    {
        if (this->node_ != nullptr)
        {
            PSYQ_ASSERT(this == this->node_->get_handle());
            this->node_->handle_ = nullptr;
            this->node_ = nullptr;
        }
    }

    /** @brief *thisが mosp_tree 空間分割木に取りつけられているか判定する。
        @retval true  *thisは mosp_tree に取りつけられている。
        @retval false *thisは mosp_tree に取りつけられていない。
        @sa attach_tree() detach_tree()
     */
    public: bool is_attached() const
    {
        return this->node_ != nullptr;
    }

    //-------------------------------------------------------------------------
    /// thisに対応する空間分割木のnode。
    private: internal::mosp_node<template_collision_object>* node_;

    /** @brief thisに対応する、衝突物体の識別値。

        この値を引数として、 mosp_tree::detect_collision()
        の引数に指定した、衝突callback関数が呼び出される。
     */
    public: template_collision_object object_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief psyq::mosp_tree で使う、単方向循環listのnode。
    @tparam template_collision_object @copydoc mosp_handle::collision_object
 */
template<typename template_collision_object>
class psyq::internal::mosp_node
{
    /// *thisの型。
    private: typedef mosp_node<template_collision_object> self;
    friend class mosp_handle<template_collision_object>;

    //-------------------------------------------------------------------------
    /// @brief 単独のnodeを構築する。
    public: mosp_node(): handle_(nullptr)
    {
        this->next_ = this;
    }

    /// copy-constructorは使用禁止。
    private: mosp_node(self const&);

    /// destructor
    public: ~mosp_node()
    {
        PSYQ_ASSERT(this->is_alone());
        auto const local_handle(this->handle_);
        if (local_handle != nullptr)
        {
            local_handle->detach_tree();
            PSYQ_ASSERT(this->handle_ == nullptr);
        }
    }

    /// copy代入演算子は使用禁止。
    private: self& operator=(self const&);

    /** @brief thisに対応する mosp_handle を取得する。
        @retval !=nullptr thisに対応する mosp_handle へのポインタ。
        @retval ==nullptr thisに対応する mosp_handle が存在しない。
     */
    public: mosp_handle<template_collision_object> const* get_handle() const
    {
        return this->handle_;
    }

    /** @brief 単独のnodeか判定する。
     */
    public: bool is_alone() const
    {
        return this == &this->get_next();
    }

    /** @brief 次のnodeを取得する。
        @return 次のnode。
     */
    public: const self& get_next() const
    {
        return *this->next_;
    }

    /// @copydoc get_next
    public: self& get_next()
    {
        return *this->next_;
    }

    /** @brief 単独のnodeを、thisの次に挿入する。

        挿入するnodeは、単独である必要がある。

        @param[in,out] io_insert_node 挿入するnode。
        @sa remove_next() is_alone()
     */
    public: void insert_next(self& io_insert_node)
    {
        // 単独の循環ノードでないと挿入できない。
        PSYQ_ASSERT(io_insert_node.is_alone());
        io_insert_node.next_ = &this->get_next();
        this->next_ = &io_insert_node;
    }

    /** @brief thisの次にあるnodeを取り外す。

        取り外したnodeは、単独のnodeとなる。

        @return 取り外したnode。
     */
    public: self& remove_next()
    {
        auto& local_next(this->get_next());
        this->next_ = &(local_next.get_next());
        local_next.next_ = &local_next;
        return local_next;
    }

    //-------------------------------------------------------------------------
    /// 単方向循環listの次の mosp_node
    private: self* next_;
    /// thisに対応する mosp_handle
    private: mosp_handle<template_collision_object>* handle_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief morton順序による空間分割木。

    使い方の概要は、以下の通り。
    -# mosp_tree::mosp_tree() を呼び出し、
       衝突判定を行う領域を空間分割木に設定する。
    -# mosp_handle::attach_tree() を呼び出し、
       mosp_handle を mosp_tree に取りつける。
    -# mosp_tree::detect_collision() を呼び出し、衝突判定を行う。

    @tparam template_collision_object @copydoc mosp_handle::collision_object
    @tparam template_space            @copydoc mosp_tree::space

    @note mosp_tree::cell_map に任意の辞書template型を指定できるようにしたい。
 */
template<typename template_collision_object, typename template_space>
class psyq::old::mosp_tree
{
    private: typedef mosp_tree self; ///< *thisの型。

    /// 空間分割木に取りつける mosp_handle 。
    public: typedef mosp_handle<template_collision_object> handle;
    /// @cond
    friend handle;
    /// @endcond

    /// 空間分割木のnode 。
    public: typedef internal::mosp_node<template_collision_object> node;

    /// 使用するmorton空間。 mosp_space_2d や mosp_space_3d を使う。
    public: typedef template_space space;

    /// cell（同じ分割空間に所属する self::node の、単方向循環list）の辞書。
    private: typedef std::map<
        typename self::space::order, typename self::node*> cell_map;
    /// self::node のmemory割当子。
    private: typedef typename self::cell_map::allocator_type::template
        rebind<typename self::node>::other
            node_allocator;
    public: enum: unsigned
    {
        /// 対応できる空間分割の最深level。
        LEVEL_LIMIT = (8 * sizeof(typename self::space::order) - 1)
            / self::space::DIMENSION
    };

    //-------------------------------------------------------------------------
    /** @brief 衝突判定を行う領域を設定する。
        @param[in] in_aabb  衝突判定を行う領域の全体を包む、絶対座標系AABB。
        @param[in] in_level 空間分割の最深レベル。
     */
    public: explicit mosp_tree(
        typename self::space::coordinates::aabb const& in_aabb,
        unsigned const in_level = self::LEVEL_LIMIT)
    :
        space_(in_aabb, in_level),
        idle_end_(nullptr),
        detect_collision_(false)
    {
        // 空間分割の最深levelを決定。
        if (in_level <= self::LEVEL_LIMIT)
        {
            this->level_cap_ = in_level;
        }
        else
        {
            // 最高level以上の空間は作れない。
            PSYQ_ASSERT(false);
            this->level_cap_ = self::LEVEL_LIMIT;
        }
    }

    /// copy-constructorは使用禁止。
    private: mosp_tree(self const&);

    /** @brief move-constructor。
        @param[in,out] io_source 移動元となるinstance。
     */
    public: mosp_tree(self&& io_source):
        cell_map_(std::move(io_source.cell_map_)),
        space_(io_source.space_),
        idle_end_(nullptr),
        level_cap_(io_source.level_cap_),
        detect_collision_(false)
    {
        io_source.cell_map_.clear();
        if (io_source.detect_collision_)
        {
            // 衝突判定中はmoveできない。
            PSYQ_ASSERT(false);
            this->cell_map_.swap(io_source.cell_map_);
        }
    }

    /** @brief 取りつけられてる mosp_handle をすべて取り外し、
               空間分割木を破棄する。
     */
    public: ~mosp_tree()
    {
        // 衝突判定中は、破棄できない。
        PSYQ_ASSERT(!this->detect_collision_);
        self::destroy_cell(this->cell_map_.get_allocator(), this->idle_end_);
        for (auto const& local_value: this->cell_map_)
        {
            self::destroy_cell(
                this->cell_map_.get_allocator(), local_value.second);
        }
    }

    /// copy代入演算子は使用禁止。
    private: self& operator=(self const&);

    /** @brief move代入演算子。
        @param[in,out] io_source 移動元となるinstance。
     */
    public: self& operator=(self&& io_source)
    {
        if (this != &io_source)
        {
            this->~self();
            new(this) self(std::move(io_source));
        }
        return *this;
    }

    //-------------------------------------------------------------------------
    /** @brief thisに取りつけられたすべての mosp_handle の衝突判定を行う。

        mosp_handle::attach_tree() によってthisに取りつけられた
        mosp_handle のうち、分割空間が重なった2つの mosp_handle の持つ
        mosp_handle::collision_object を引数に、衝突callback関数を呼び出す。

        @param[in] in_detect_callback
            衝突callback関数。引数として、2つの mosp_handle::collision_object
            を受け取ること。戻り値はなくてよい。
            2つの mosp_handle が所属する分割空間が重なったとき、呼び出される。

        @retval ture 成功。衝突判定を行った。
        @retval false
            失敗。衝突判定を行わなかった。
            原因は、すでに衝突判定を行なってる最中だから。
     */
    public: template<typename template_detect_callback>
    bool detect_collision(template_detect_callback const& in_detect_callback)
    {
        // 衝突判定を開始する。
        auto const local_cell_map(this->begin_detect());
        if (local_cell_map == nullptr)
        {
            return false;
        }

        // cellごとに衝突判定を行う。
        for (auto& local_value: *local_cell_map)
        {
            PSYQ_ASSERT(local_value.second != nullptr);
            self::detect_cell(
                *local_value.second,
                local_value.first,
                *local_cell_map,
                in_detect_callback);
        }

        // 衝突判定を終了する。
        this->end_detect();
        return true;
    }

    //-------------------------------------------------------------------------
    /** @brief 空間分割木の衝突判定を開始する。

        end_detect() と対になるように呼び出すこと。

        @retval !=nullptr 衝突判定を行うcellの辞書。 detect_cell() で使う。
        @retval ==nullptr 失敗。すでに begin_detect() が呼び出されていた。
     */
    private: typename self::cell_map const* begin_detect()
    {
        if (this->detect_collision_)
        {
            return nullptr;
        }
        this->idle_end_ = self::collect_idle_node(
            this->idle_end_, this->cell_map_);
        this->detect_collision_ = true;
        return &this->cell_map_;
    }

    /** @brief 空間分割木の衝突判定を終了する。

        begin_detect() と対になるように呼び出すこと。
     */
    private: void end_detect()
    {
        PSYQ_ASSERT(this->detect_collision_);
        this->detect_collision_ = false;
    }

    /** @brief cellとcellの辞書とで、衝突判定を行う。

        begin_detect() と end_detect() の間で呼び出すこと。

        @param[in] in_cell_end   衝突判定を行うcellの終端node。
        @param[in] in_cell_order 衝突判定を行うcellのmorton順序。
        @param[in] in_cell_map   衝突の対象となるcellの辞書。
        @param[in] in_detect_callback
            衝突callback関数。戻り値はなくてよい。
            引数として、2つの mosp_handle::collision_object を受け取ること。
            2つの mosp_handle が所属する分割空間が重なったとき、呼び出される。
     */
    private: template<typename template_detect_callback>
    static void detect_cell(
        typename self::node const&        in_cell_end,
        typename self::space::order const in_cell_order,
        typename self::cell_map const&    in_cell_map,
        template_detect_callback const&   in_detect_callback)
    {
        // cellの内部だけで、衝突判定を行う。
        for (
            auto local_node(&in_cell_end.get_next());
            local_node != &in_cell_end;
            local_node = &local_node->get_next())
        {
            self::detect_node_and_cell(
                *local_node,
                local_node->get_next(),
                in_cell_end,
                in_detect_callback);
        }

        // cellと上位のcellとで、衝突判定を行う。
        for (auto local_super_order(in_cell_order); 0 < local_super_order;)
        {
            // 上位のcellへ移動し、衝突判定を行う。
            local_super_order
                = (local_super_order - 1) >> self::space::DIMENSION;
            auto const local_super_iterator(
                in_cell_map.find(local_super_order));
            if (local_super_iterator == in_cell_map.end())
            {
                // 上位のcellがなかったので、次へ。
                continue;
            }
            auto const& local_super_cell_end(*local_super_iterator->second);
            for (
                auto local_node(&in_cell_end.get_next());
                local_node != &in_cell_end;
                local_node = &local_node->get_next())
            {
                self::detect_node_and_cell(
                    *local_node,
                    local_super_cell_end.get_next(),
                    local_super_cell_end,
                    in_detect_callback);
            }
        }
    }

    /** @brief nodeとcellとで、衝突判定を行う。
        @param[in] in_node       衝突判定を行うnode。
        @param[in] in_cell_begin 衝突判定を行うcellの先頭node。
        @param[in] in_cell_end   衝突判定を行うcellの終端node。
        @param[in] in_detect_callback
            衝突callback関数。戻り値はなくてよい。
            引数として、2つの mosp_handle::collision_object を受け取ること。
            2つの mosp_handle が所属する分割空間が重なったとき、呼び出される。
     */
    private: template<typename template_detect_callback>
    static void detect_node_and_cell(
        typename self::node const&      in_node,
        typename self::node const&      in_cell_begin,
        typename self::node const&      in_cell_end,
        template_detect_callback const& in_detect_callback)
    {
        auto local_node_handle(in_node.get_handle());
        if (local_node_handle == nullptr)
        {
            return;
        }
        for (
            auto local_cell_node(&in_cell_begin);
            local_cell_node != &in_cell_end;
            local_cell_node = &local_cell_node->get_next())
        {
            auto const local_cell_node_handle(local_cell_node->get_handle());
            if (local_cell_node_handle == nullptr)
            {
                continue;
            }

            // 衝突callback関数を呼び出す。
            in_detect_callback(
                local_node_handle->object_,
                local_cell_node_handle->object_);

            // 衝突callback関数の中で handle::detach_tree()
            // される場合があるので、 handle を再取得する。
            local_node_handle = in_node.get_handle();
            if (local_node_handle == nullptr)
            {
                return;
            }
        }
    }

    //-------------------------------------------------------------------------
    /** @brief AABBを包む最小の分割空間のnodeを構築する。
        @param[in] in_min 絶対座標系AABBの最小値。
        @param[in] in_max 絶対座標系AABBの最大値。
        @retval !=nullptr AABBを包む最小の分割空間のnode。
        @retval ==nullptr 失敗。
     */
    private: typename self::node* make_node(
        typename self::space::coordinates::aabb const& in_aabb)
    {
        if (this->detect_collision_)
        {
            // detect_collision() の実行中はnodeを作れない。
            PSYQ_ASSERT(false);
            return nullptr;
        }

        // morton順序に対応するcellを用意する。
        auto const local_morton_order(
            self::calc_order(this->level_cap_, this->space_, in_aabb));
        auto& local_cell(this->cell_map_[local_morton_order]);
        if (local_cell == nullptr)
        {
            local_cell = this->distribute_idle_node();
            if (local_cell == nullptr)
            {
                PSYQ_ASSERT(false);
                this->cell_map_.erase(local_morton_order);
                return nullptr;
            }
        }

        // morton順序に対応するnodeを用意する。
        auto const local_node(this->distribute_idle_node());
        if (local_node == nullptr)
        {
            PSYQ_ASSERT(false);
            return nullptr;
        }
        local_cell->insert_next(*local_node);
        return local_node;
    }

    //-------------------------------------------------------------------------
    /** @brief AABBを包む最小の分割空間のmorton順序を算出する。
        @param[in] in_level_cap 空間分割の最深level。
        @param[in] in_space     使用するmorton空間。
        @param[in] in_min       絶対座標系AABBの最小値。
        @param[in] in_max       絶対座標系AABBの最大値。
        @return AABBを包む最小の分割空間のmorton順序。
     */
    private: static typename self::space::order calc_order(
        std::size_t const                              in_level_cap,
        typename self::space const&                    in_space,
        typename self::space::coordinates::aabb const& in_aabb)
    {
        if (in_level_cap <= 0)
        {
            return 0;
        }

        // 衝突物体のAABBを包む、最小の分割空間のmorton順序を算出する。
        unsigned const local_axis_order_max((1 << in_level_cap) - 1);
        auto const local_min_morton(
            in_space.calc_order(in_aabb.get_min(), local_axis_order_max));
        auto const local_max_morton(
            in_space.calc_order(in_aabb.get_max(), local_axis_order_max));
        auto const local_morton_distance(local_max_morton ^ local_min_morton);
        unsigned local_level;
        if (local_morton_distance != 0)
        {
            auto const local_clz(
                psyq::count_leading_0bits(local_morton_distance));
            local_level = (sizeof(local_morton_distance) * 8 + 1 - local_clz)
                / self::space::DIMENSION;
        }
        else
        {
            local_level = 1;
        }
        unsigned const local_cell_count(
            1 << ((in_level_cap - local_level) * self::space::DIMENSION));
        unsigned const local_base(
            (local_cell_count - 1) / ((1 << self::space::DIMENSION) - 1));
        auto const local_shift(local_level * self::space::DIMENSION);
        /** @note
            bit数以上のbit-right-shift演算は、
            C言語の仕様として未定義の動作となる。
            http://hexadrive.sblo.jp/article/56575654.html
         */
        PSYQ_ASSERT(local_shift < sizeof(local_max_morton) * 8);
        return local_base + (local_max_morton >> local_shift);
    }

    //-------------------------------------------------------------------------
    /** @brief 空きnodeを配布する。
        @retval !=nullptr 配布された空きnode。
        @retval ==nullptr 失敗。
     */
    private: typename self::node* distribute_idle_node()
    {
        auto const local_idle_end(this->idle_end_);
        if (local_idle_end == nullptr)
        {
            // 空きnodeがなかったので、新たにnodeを生成する。
            return self::create_node(this->cell_map_.get_allocator());
        }

        // 空きnodeを取り出す。
        if (local_idle_end->is_alone())
        {
            this->idle_end_ = nullptr;
            return local_idle_end;
        }
        return &local_idle_end->remove_next();
    }

    /** @brief cellの辞書から空きnodeを回収し、空きlistに追加する。
        @param[in,out] io_idle_end 空きlistの終端node。
        @param[in,out] io_cell_map 空きnodeが回収される、cellの辞書。
        @return 空きlistの新たな終端node。
     */
    private: static typename self::node* collect_idle_node(
        typename self::node* const io_idle_end,
        typename self::cell_map&   io_cell_map)
    {
        // 空きlistを用意する。
        auto local_idle_end(io_idle_end);
        if (local_idle_end == nullptr)
        {
            local_idle_end = self::create_node(io_cell_map.get_allocator());
            if (local_idle_end == nullptr)
            {
                return nullptr;
            }
        }

        // cellの辞書から空きnodeを回収し、空きlistに追加する。
        for (auto i(io_cell_map.cbegin()); i != io_cell_map.cend();)
        {
            auto& local_cell_end(*(i->second));
            local_idle_end = self::collect_idle_node_in_cell(
                *local_idle_end, local_cell_end);
            if (local_cell_end.is_alone())
            {
                // cellが空になったので、
                // cellの終端nodeも空きlistに追加し、cellを削除する。
                local_idle_end->insert_next(local_cell_end);
                local_idle_end = &local_cell_end;
                i = io_cell_map.erase(i);
            }
            else
            {
                ++i;
            }
        }
        return local_idle_end;
    }

    /** @brief cellから空きnodeを回収し、空きlistに追加する。
        @param[in,out] io_idle_end 空きlistの終端node。
        @param[in,out] io_cell_end 空きnodeが回収される、cellの終端node。
        @return 空きlistの新たな終端node。
     */
    private: static typename self::node* collect_idle_node_in_cell(
        typename self::node& io_idle_end,
        typename self::node& io_cell_end)
    {
        auto local_idle_end(&io_idle_end);
        auto local_last_node(&io_cell_end);
        for (
            auto local_node(&io_cell_end.get_next());
            local_node != &io_cell_end;
            local_node = &local_last_node->get_next())
        {
            if (local_node->get_handle() != nullptr)
            {
                // 空きnodeではないので、次のnodeへ移行する。
                local_last_node = local_node;
            }
            else
            {
                // 空きnodeだったのでcellから削除し、
                // 空きlistの末尾に追加する。
                auto& local_idle_node(local_last_node->remove_next());
                local_idle_end->insert_next(local_idle_node);
                local_idle_end = &local_idle_node;
            }
        }
        return local_idle_end;
    }

    //-------------------------------------------------------------------------
    private: static typename self::node* create_node(
        typename self::node_allocator io_allocator)
    {
        auto const local_node(io_allocator.allocate(1));
        if (local_node == nullptr)
        {
            PSYQ_ASSERT(false);
            return nullptr;
        }
        return new(local_node) typename self::node;
    }

    /** @brief cellが持つnodeを、すべて破棄する。
        @param[in,out] io_allocator nodeの破棄に使うmemory割当子。
        @param[in,out] io_cell_end  破棄するcellの終端node。
     */
    private: static void destroy_cell(
        typename self::node_allocator io_allocator,
        typename self::node* const    io_cell_end)
    {
        if (io_cell_end != nullptr)
        {
            for (;;)
            {
                auto& local_node(io_cell_end->remove_next());
                local_node.~node();
                io_allocator.deallocate(&local_node, 1);
                if (io_cell_end == &local_node)
                {
                    break;
                }
            }
        }
    }

    //-------------------------------------------------------------------------
    private: typename self::cell_map cell_map_; ///< @copydoc cell_map
    private: typename self::space space_; ///< @copydoc space
    private: typename self::node* idle_end_; ///< 空きlistの終端node。
    private: std::size_t level_cap_; ///< 空間分割の最深level。
    private: bool detect_collision_; ///< detect_collision() を実行中かどうか。
};

#endif // !defined(PSYQ_MOSP_TREE_OLD_HPP_)