#pragma once

// 定义Map类
template<typename Key, typename Val>
class Map final {
public:
    Map();// 构造函数, 初始化一个空的红黑树
    Map(const Map &);// 拷贝构造函数
    ~Map();// 析构函数, 释放红黑树占用的内存空间
    Map &operator=(const Map &);// 赋值函数重载
    bool contain(const Key &);// 查找接口函数, 对外提供查找功能, 返回一个布尔值, 表示给定的数据是否存在于红黑树中
    void remove(const Key &);// 删除接口函数, 对外提供删除功能, 将一个给定的数据从红黑树中删除, 如果不存在则不删除
    Val &operator[](const Key &);//获取元素
    void clear();//清空树
    int size();// 定义获取红黑树所有节点个数的函数
private:
    class RBNode final {
    public:
        Key key; // 节点存储键值
        Val val; // 节点存储的数据
        enum Color {
            RED, BLACK
        } color; // 节点的颜色
        RBNode *left; // 左子节点指针
        RBNode *right; // 右子节点指针
        RBNode *parent; // 父节点指针
        RBNode(const Key &, const Val &, Color color);// 构造函数, 初始化节点的数据和颜色, 以及子节点和父节点指针
    };
    RBNode *root = nullptr; // 根节点指针
    void destroy(RBNode *);// 销毁函数, 用于递归地销毁一个给定的子树, 并释放其内存空间
    void leftRotate(RBNode *);// 左旋函数, 将给定节点作为旋转轴, // 将其右子节点提升为新的父节点, 并调整相关指针和颜色
    void rightRotate(RBNode *);// 右旋函数, 将给定节点作为旋转轴, 将其左子节点提升为新的父节点, 并调整相关指针和颜色
    void insertFixup(RBNode *);// 插入修复函数, 用于在插入新节点后, 修复红黑树的性质
    void deleteFixup(RBNode *);// 删除修复函数, 用于在删除一个节点后, 修复红黑树的性质
    RBNode *search(const Key &);// 查找函数, 用于在红黑树中查找给定的数据, 返回对应的节点指针, 如果不存在则返回空指针
    void insert(const Key &, const Val &);// 插入函数, 用于在红黑树中插入一个新的数据, 如果已经存在则不插入
    void transplant(RBNode *, RBNode *);// 替换函数, 用于在红黑树中用一个子树替换另一个子树, 保持相关指针和颜色不变
    static RBNode *minNum(RBNode *);// 最小值函数, 用于在红黑树中找到一个给定子树的最小值节点, 返回其指针
    int size(RBNode *);//递归计算节点个数
    RBNode* cloneNode(RBNode *); // 克隆辅助函数
};

template<typename Key, typename Val>
Map<Key, Val>::Map() = default;

template<typename Key, typename Val>
Map<Key, Val>::~Map() {
    destroy(root);
}

template<typename Key, typename Val>
void Map<Key, Val>::destroy(RBNode *x) {
    if (x != nullptr) { // 如果当前节点不为空, 继续销毁其左右子树, 并释放其内存空间
        destroy(x->left);
        destroy(x->right);
        delete x;
    }
}

template<typename Key, typename Val>
void Map<Key, Val>::leftRotate(RBNode *x)  {
    RBNode *y = x->right; // 右子节点作为新的父节点
    x->right = y->left; // 将右子节点的左子树接到旋转轴的右边
    if (y->left != nullptr) // 如果右子节点有左子树, 更新其父指针
        y->left->parent = x;
    y->parent = x->parent; // 更新右子节点的父指针
    if (x->parent == nullptr) // 如果旋转轴是根节点, 更新根指针
        root = y;
    else if (x == x->parent->left) // 如果旋转轴是其父节点的左子节点, 更新其父节点的左子指针
        x->parent->left = y;
    else // 如果旋转轴是其父节点的右子节点, 更新其父节点的右子指针
        x->parent->right = y;
    y->left = x; // 将旋转轴接到右子节点的左边
    x->parent = y; // 更新旋转轴的父指针
}

template<typename Key, typename Val>
void Map<Key, Val>::rightRotate(RBNode *x) {
    RBNode *y = x->left; // 左子节点作为新的父节点
    x->left = y->right; // 将左子节点的右子树接到旋转轴的左边
    if (y->right != nullptr) // 如果左子节点有右子树, 更新其父指针
        y->right->parent = x;
    y->parent = x->parent; // 更新左子节点的父指针
    if (x->parent == nullptr) // 如果旋转轴是根节点, 更新根指针
        root = y;
    else if (x == x->parent->right) // 如果旋转轴是其父节点的右子节点, 更新其父节点的右子指针
        x->parent->right = y;
    else // 如果旋转轴是其父节点的左子节点, 更新其父节点的左子指针
        x->parent->left = y;
    y->right = x; // 将旋转轴接到左子节点的右边
    x->parent = y; // 更新旋转轴的父指针
}

template<typename Key, typename Val>
void Map<Key, Val>::insertFixup(RBNode *z) {
    while (z->parent != nullptr && z->parent->color == RBNode::RED) { // 如果新节点的父节点是红色, 需要进行调整
        if (z->parent == z->parent->parent->left) { // 如果新节点的父节点是其祖父节点的左子节点
            RBNode *y = z->parent->parent->right; // 找到新节点的叔叔节点
            if (y != nullptr && y->color == RBNode::RED) { // 如果叔叔节点是红色, 情况一
                z->parent->color = RBNode::BLACK; // 将父节点和叔叔节点染成黑色
                y->color = RBNode::BLACK;
                z->parent->parent->color = RBNode::RED; // 将祖父节点染成红色
                z = z->parent->parent; // 将新节点上移两层, 继续检查
            } else { // 如果叔叔节点是黑色或者不存在
                if (z == z->parent->right) { // 如果新节点是其父节点的右子节点, 情况二
                    z = z->parent; // 将新节点上移一层
                    leftRotate(z); // 对新节点进行左旋, 转化为情况三
                }
                // 如果新节点是其父节点的左子节点, 情况三
                z->parent->color = RBNode::BLACK; // 将父节点染成黑色
                z->parent->parent->color = RBNode::RED; // 将祖父节点染成红色
                rightRotate(z->parent->parent); // 对祖父节点进行右旋, 完成调整
            }
        } else { // 如果新节点的父节点是其祖父节点的右子节点, 对称处理
            RBNode *y = z->parent->parent->left; // 找到新节点的叔叔节点
            if (y != nullptr && y->color == RBNode::RED) { // 如果叔叔节点是红色, 情况一
                z->parent->color = RBNode::BLACK; // 将父节点和叔叔节点染成黑色
                y->color = RBNode::BLACK;
                z->parent->parent->color = RBNode::RED; // 将祖父节点染成红色
                z = z->parent->parent; // 将新节点上移两层, 继续检查
            } else { // 如果叔叔节点是黑色或者不存在
                if (z == z->parent->left) { // 如果新节点是其父节点的左子节点, 情况二
                    z = z->parent; // 将新节点上移一层
                    rightRotate(z); // 对新节点进行右旋, 转化为情况三
                }
                // 如果新节点是其父节点的右子节点, 情况三
                z->parent->color = RBNode::BLACK; // 将父节点染成黑色
                z->parent->parent->color = RBNode::RED; // 将祖父节点染成红色
                leftRotate(z->parent->parent); // 对祖父节点进行左旋, 完成调整
            }
        }
    }
    root->color = RBNode::BLACK; // 保证根节点是黑色
}

template<typename Key, typename Val>
void Map<Key, Val>::deleteFixup(RBNode *x) {
    while (x != root && x != nullptr && x->color == RBNode::BLACK) { // 如果被删除的原始位置是黑色, 需要进行调整
        if (x == x->parent->left) { // 如果被删除的原始位置是其父亲的左子位置 
            RBNode *w = x->parent->right; // 找到兄弟位置 
            if (w->color == RBNode::RED) {  // 情况一：兄弟位置是红色 
                w->color =                     RBNode::BLACK; // 将兄弟位置染成黑色
                x->parent->color = RBNode::RED; // 将父亲位置染成红色
                leftRotate(x->parent); // 对父亲位置进行左旋, 转化为情况二、三或四
                w = x->parent->right; // 更新兄弟位置
            }
            if (w->left->color == RBNode::BLACK && w->right->color == RBNode::BLACK) { // 情况二：兄弟位置是黑色, 且其两个子位置都是黑色
                w->color = RBNode::RED; // 将兄弟位置染成红色
                x = x->parent; // 将原始位置上移一层, 继续检查
            } else { // 情况三或四：兄弟位置是黑色, 且至少有一个子位置是红色
                if (w->right->color == RBNode::BLACK) { // 情况三：兄弟位置的右子位置是黑色, 左子位置是红色
                    w->left->color = RBNode::BLACK; // 将兄弟位置的左子位置染成黑色
                    w->color = RBNode::RED; // 将兄弟位置染成红色
                    rightRotate(w); // 对兄弟位置进行右旋, 转化为情况四
                    w = x->parent->right; // 更新兄弟位置
                }
                // 情况四：兄弟位置的右子位置是红色, 左子位置任意颜色
                w->color = x->parent->color; // 将兄弟位置染成父亲位置的颜色
                x->parent->color = RBNode::BLACK; // 将父亲位置染成黑色
                w->right->color = RBNode::BLACK; // 将兄弟位置的右子位置染成黑色
                leftRotate(x->parent); // 对父亲位置进行左旋, 完成调整
                x = root; // 将原始位置设为根节点, 结束循环
            }
        } else { // 如果被删除的原始位置是其父亲的右子位置, 对称处理
            RBNode *w = x->parent->left; // 找到兄弟位置 
            if (w->color == RBNode::RED) {  // 情况一：兄弟位置是红色 
                w->color = RBNode::BLACK; // 将兄弟位置染成黑色
                x->parent->color = RBNode::RED; // 将父亲位置染成红色
                rightRotate(x->parent); // 对父亲位置进行右旋, 转化为情况二、三或四
                w = x->parent->left; // 更新兄弟位置
            }
            if (w->right->color == RBNode::BLACK && w->left->color == RBNode::BLACK) { // 情况二：兄弟位置是黑色, 且其两个子位置都是黑色
                w->color = RBNode::RED; // 将兄弟位置染成红色
                x = x->parent; // 将原始位置上移一层, 继续检查
            } else { // 情况三或四：兄弟位置是黑色, 且至少有一个子位置是红色
                if (w->left->color == RBNode::BLACK) {  // 情况三：兄弟位置的左子位置是黑色, 右子位置是红色 
                    w->right->color = RBNode::BLACK;  // 将兄弟位置的右子位染成黑色 
                    w->color = RBNode::RED;  // 将兄弟位染成红色 
                    leftRotate(w);  // 对兄弟位进行左旋, 转化为情况四 
                    w = x->parent->left;  // 更新兄弟位 
                }
                // 情况四：兄弟位的左子位是红色, 右子位任意颜色 
                w->color = x->parent->color;  // 将兄弟位染成父亲位的颜色 
                x->parent->color = RBNode::BLACK;  // 将父亲位染成黑色 
                w->left->color = RBNode::BLACK;  // 将兄弟位的左子位染成黑色 
                rightRotate(x->parent);  // 对父亲位进行右旋, 完成调整 
                x = root;  // 将原始位设为根节点, 结束循环 
            }
        }
    }
    if (x != nullptr) // 如果原始位置不为空, 将其染成黑色
        x->color = RBNode::BLACK;
}

template<typename Key, typename Val>
typename Map<Key, Val>::RBNode *Map<Key, Val>::search(const Key &key) {
    RBNode *x = root; // 从根节点开始查找
    while (x != nullptr && x->key != key) // 如果当前节点不为空且不等于目标数据, 继续查找
        if (key < x->key) // 如果目标数据小于当前节点的数据, 向左子树查找
            x = x->left;
        else // 如果目标数据大于当前节点的数据, 向右子树查找
            x = x->right;
    return x; // 返回查找结果
}

template<typename Key, typename Val>
void Map<Key, Val>::insert(const Key &key, const Val &val) {
    auto *z = new RBNode(key, val, RBNode::RED); // 创建一个新的红色节点
    RBNode *y = nullptr; // 初始化一个空指针作为新节点的父节点
    RBNode *x = root; // 从根节点开始查找插入位置
    while (x != nullptr) { // 如果当前节点不为空, 继续查找
        y = x; // 记录当前节点作为新节点的父节点
        if (key < x->key) // 如果新节点的数据小于当前节点的数据, 向左子树查找
            x = x->left;
        else if (key > x->key) // 如果新节点的数据大于当前节点的数据, 向右子树查找
            x = x->right;
        else // 如果新节点的数据等于当前节点的数据, 说明已经存在, 不需要插入, 直接返回
            return;
    }
    z->parent = y; // 将新节点的父指针指向找到的父节点
    if (y == nullptr) // 如果父节点为空, 说明红黑树为空, 将新节点设为根节点
        root = z;
    else if (key < y->key) // 如果新节点的数据小于父节点的数据, 将新节点设为父节点的左子节点
        y->left = z;
    else // 如果新节点的数据大于父节点的数据, 将新节点设为父节点的右子节点
        y->right = z;
    insertFixup(z); // 调用插入修复函数, 修复红黑树的性质
}

template<typename Key, typename Val>
void Map<Key, Val>::remove(const Key &key) {
    RBNode *z = search(key); // 调用查找函数, 找到要删除的目标节点
    if (z == nullptr) // 如果目标节点不存在, 直接返回
        return;
    RBNode *y = z; // 记录要删除的原始位置
    RBNode *x; // 记录要删除的原始位置的后继位置
    typename RBNode::Color yOriginalColor = y->color; // 记录要删除的原始位置的颜色
    if (z->left == nullptr) { // 如果目标节点没有左子树, 用其右子树替代它
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == nullptr) { // 如果目标节点没有右子树
        x = z->left; // 用其左子树替代它
        transplant(z, z->left);
    } else { // 如果目标节点有左右子树
        y = minNum(z->right); // 找到其右子树中的最小节点, 作为替代节点
        yOriginalColor = y->color; // 记录替代节点的颜色
        x = y->right; // 记录替代节点的右子树
        if (y->parent == z) { // 如果替代节点是目标节点的右子节点
            if (x != nullptr) { // 如果替代节点有右子树, 更新其父指针
                x->parent = y;
            }
        } else { // 如果替代节点不是目标节点的右子节点
            transplant(y, y->right); // 用替代节点的右子树替换替代节点
            y->right = z->right; // 将目标节点的右子树接到替代节点的右边
            y->right->parent = y; // 更新右子树的父指针
        }
        transplant(z, y); // 用替代节点替换目标节点
        y->left = z->left; // 将目标节点的左子树接到替代节点的左边
        y->left->parent = y; // 更新左子树的父指针
        y->color = z->color; // 将替代节点染成目标节点的颜色
    }
    delete z; // 释放目标节点的内存空间
    if (yOriginalColor == RBNode::BLACK) // 如果被删除的原始位置是黑色, 调用删除修复函数, 修复红黑树的性质
        deleteFixup(x);
}

template<typename Key, typename Val>
void Map<Key, Val>::transplant(RBNode *u, RBNode *v) {
    if (u->parent == nullptr) // 如果要被替换的子树是根节点, 将新的子树设为根节点
        root = v;
    else if (u == u->parent->left) // 如果要被替换的子树是其父节点的左子树, 将新的子树设为父节点的左子树
        u->parent->left = v;
    else // 如果要被替换的子树是其父节点的右子树, 将新的子树设为父节点的右子树
        u->parent->right = v;
    if (v != nullptr) // 如果新的子树不为空, 更新其父指针
        v->parent = u->parent;
}

template<typename Key, typename Val>
typename Map<Key, Val>::RBNode *Map<Key, Val>::minNum(RBNode *x) {
    while (x != nullptr && x->left != nullptr) // 如果当前节点不为空且有左子树, 继续向左查找
        x = x->left;
    return x; // 返回查找结果
}

template<typename Key, typename Val>
Map<Key, Val>::RBNode::RBNode(const Key &key, const Val &val, Color color) {
    this->key = key;
    this->val = val;
    this->color = color;
    this->left = nullptr;
    this->right = nullptr;
    this->parent = nullptr;
}

template<typename Key, typename Val>
Val &Map<Key, Val>::operator[](const Key &key) {
    if (!contain(key))
        insert(key, Val());
    return search(key)->val;
}
template<typename Key, typename Val>
void Map<Key, Val>::clear() {
    destroy(root);
    root = nullptr;
}
template<typename Key, typename Val>
bool Map<Key, Val>::contain(const Key &key) {
    RBNode *x = search(key); // 调用查找函数, 找到对应的节点指针
    return x != nullptr; // 如果节点指针不为空, 说明存在, 返回真, 否则返回假
}

template<typename Key, typename Val>
int Map<Key, Val>::size() {
    return size(root); // 调用递归函数, 从根节点开始计算
}

template<typename Key, typename Val>
int Map<Key, Val>::size(RBNode *x) {
    if (x == nullptr) // 如果当前节点为空, 返回0
        return 0;
    else // 如果当前节点不为空, 返回1加上其左右子树的节点个数之和
        return 1 + size(x->left) + size(x->right);
}

// 拷贝构造函数
template<typename Key, typename Val>
Map<Key, Val>::Map(const Map &other) : root(nullptr)
{
    RBNode *otherRoot = other.root;
    if (otherRoot != nullptr)
        root = cloneNode(otherRoot);
}

// 深拷贝辅助函数
template<typename Key, typename Val>
typename Map<Key, Val>::RBNode* Map<Key, Val>::cloneNode(RBNode *node)
{
    if (node == nullptr) return nullptr;

    // 创建新节点
    auto newNode = new RBNode(node->key, node->val, node->color);

    // 深拷贝左右子树
    newNode->left = cloneNode(node->left);
    newNode->right = cloneNode(node->right);

    // 设置子节点的父节点指针
    if (newNode->left != nullptr) newNode->left->parent = newNode;
    if (newNode->right != nullptr) newNode->right->parent = newNode;

    return newNode;
}

// 赋值操作符重载
template<typename Key, typename Val>
Map<Key, Val>& Map<Key, Val>::operator=(const Map &other)
{
    if (this != &other) { // 避免自赋值
        // 先销毁当前的树
        destroy(root);
        root = nullptr;
        // 再进行深拷贝
        RBNode *otherRoot = other.root;
        if (otherRoot != nullptr)
            root = cloneNode(otherRoot);
    }
    return *this;
}
