#pragma once

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

class Rope {
public:
    static constexpr size_t npos = static_cast<size_t>(-1);

    Rope()
        : Rope("")
    {
    }

    Rope(std::string const& str)
        : root(std::make_unique<Node>(str))
    {
    }

    Rope(Rope const& src)
    {
        auto new_root = Node(*src.root);
        root = std::make_unique<Node>(new_root);
    }

    Rope& operator=(Rope const& rhs)
    {
        if (this != &rhs) {
            // delete existing rope to recover memory
            root.reset();
            // invoke copy constructor
            root = std::make_unique<Node>(*(rhs.root.get()));
        }

        return *this;
    }

    size_t length() const
    {
        if (root == nullptr) {
            return 0;
        }

        return root->length();
    }

    bool empty() const
    {
        return length() == 0;
    }

    const char& at(size_t pos) const
    {
        if (root == nullptr || pos >= length()) {
            std::stringstream ss;
            ss << "Rope::at: pos (which is " << pos << ") > this->length() (which is " << length() << ')';
            throw std::out_of_range(ss.str());
        }

        return root->at(pos);
    }

    char& at(size_t pos)
    {
        if (root == nullptr || pos >= length()) {
            std::stringstream ss;
            ss << "Rope::at: pos (which is " << pos << ") > this->length() (which is " << length() << ')';
            throw std::out_of_range(ss.str());
        }

        return root->at(pos);
    }

    std::string to_string() const
    {
        if (root == nullptr) {
            return std::string{};
        }

        return root->tree_to_string();
    }

    std::string substring(size_t start, size_t len) const
    {
        auto total_length = length();
        if (start > total_length || (start + len) > total_length) {
            throw std::invalid_argument("rope index out of bounds");
        }

        return root->substring(start, len);
    }

    // A rope is balanced if and only if its length is greater than or equal to
    // fib(d+2) where d refers to the depth of the rope and fib(n) represents
    // the nth fibonacci number i.e. in the set {1, 1, 2, 3, 5, 8, etc...}
    bool is_balanced() const
    {
        if (root == nullptr)
            return true;

        auto d = root->depth();
        return length() >= fib(d + 2);
    }

    void balance()
    {
        // initiate rebalancing only if rope is unbalanced
        if (!is_balanced()) {
            // build vector representation of Fibonacci intervals
            auto intervals = build_fib_list(length());
            std::vector<Node::Handle> nodes(intervals.size());

            // get leaf nodes
            std::vector<Node*> leaves;
            root->get_leaves(leaves);

            size_t i;
            auto max_i = intervals.size() - 1;
            size_t currMaxInterval = 0;
            Node::Handle acc = nullptr;
            Node::Handle tmp = nullptr;

            // attempt to insert each leaf into nodes vector based on length
            for (auto& leaf : leaves) {
                auto len = leaf->length();
                bool inserted = false;

                // ignore empty leaf nodes
                if (len > 0) {
                    acc = std::make_unique<Node>(*leaf);
                    i = 0;

                    while (!inserted) {
                        // find appropriate slot for the acc node to be inserted,
                        // concatenating with nodes encountered along the way
                        while (i < max_i && len >= intervals[i + 1]) {
                            if (nodes[i].get() != nullptr) {
                                // concatenate encountered entries with node to be inserted
                                tmp = std::make_unique<Node>(*nodes[i].get());
                                acc = std::make_unique<Node>(*acc.get());
                                acc = std::make_unique<Node>(std::move(tmp), std::move(acc));

                                // update len
                                len = acc->length();

                                // if new length is sufficiently great that the node must be
                                //   moved to a new slot, we clear out the existing entry
                                if (len >= intervals[i + 1])
                                    nodes[i] = nullptr;
                            }

                            i++;
                        }

                        // target slot found -- check if occupied
                        if (nodes[i].get() == nullptr) {
                            nodes[i].swap(acc);
                            inserted = true;

                            // update currMaxInterval if necessary
                            if (i > currMaxInterval)
                                currMaxInterval = i;
                        }
                        else {
                            // concatenate encountered entries with node to be inserted
                            tmp = std::make_unique<Node>(*nodes[i].get());
                            acc = std::make_unique<Node>(*acc.get());
                            acc = std::make_unique<Node>(std::move(tmp), std::move(acc));

                            // update len
                            len = acc->length();

                            // if new length is sufficiently great that the node must be
                            //   moved to a new slot, we clear out the existing entry
                            if (len >= intervals[i + 1])
                                nodes[i] = nullptr;
                        }
                    }
                }
            }

            // concatenate remaining entries to produce balanced rope
            acc = std::move(nodes[currMaxInterval]);
            for (int idx = currMaxInterval; idx >= 0; idx--) {
                if (nodes[idx] != nullptr) {
                    tmp = std::make_unique<Node>(*nodes[idx].get());
                    acc = std::make_unique<Node>(std::move(acc), std::move(tmp));
                }
            }

            root = std::move(acc);
        }
    }

    Rope& prepend(std::string const& str)
    {
        return prepend(Rope{ str });
    }

    Rope& prepend(Rope const& rope)
    {
        return insert(0, rope);
    }

    Rope& insert(size_t index, std::string const& str)
    {
        return insert(index, Rope(str));
    }

    Rope& insert(size_t index, Rope const& rope)
    {
        if (index > length()) {
            std::stringstream ss;
            ss << "Rope::insert: index (which is " << index << ") > this->length() (which is " << length() << ')';
            throw std::out_of_range(ss.str());
        }

        Rope tmp{ rope };
        auto orig_split = Node::split(std::move(root), index);
        auto concat = std::make_unique<Node>(std::move(orig_split.first), std::move(tmp.root));
        root = std::make_unique<Node>(std::move(concat), std::move(orig_split.second));

        return *this;
    }

    Rope& append(std::string const& str)
    {
        Rope tmp{ str };
        root = std::make_unique<Node>(std::move(root), std::move(tmp.root));
        return *this;
    }

    Rope& append(Rope const& r)
    {
        Rope tmp{ r };
        root = std::make_unique<Node>(std::move(root), std::move(tmp.root));
        return *this;
    }

    size_t find(char needle)
    {
        if (root == nullptr) {
            return npos;
        }

        for (size_t pos = 0; pos < length(); ++pos) {
            if (at(pos) == needle) {
                return pos;
            }
        }

        return npos;
    }

    size_t search(std::string const& needle, size_t skip = 0)
    {
        if (skip >= length()) {
            return npos;
        }

        auto table = preprocess(needle);
        while (length() - skip >= needle.length()) {
            auto i = needle.length() - 1;
            while (at(skip + i) == needle[i]) {
                if (i == 0) {
                    return skip;
                }
                i -= 1;
            }
            skip += table[at(skip + needle.length() - 1)];
        }

        return npos;
    }

    void erase(size_t index = 0, size_t count = npos)
    {
        auto size = length();
        if (index > size) {
            std::stringstream ss;
            ss << "Rope::erase: index (which is " << index << ") > this->length() (which is " << size << ')';
            throw std::out_of_range(ss.str());
        }

        count = std::min(count, size - index);
        if (count > 0) {
            auto first = Node::split(std::move(root), index);
            auto second = Node::split(std::move(first.second), count);
            second.first.reset();
            root = std::make_unique<Node>(std::move(first.first), std::move(second.second));
        }
    }

    bool operator==(Rope const& rhs) const
    {
        return to_string() == rhs.to_string();
    }

    bool operator!=(Rope const& rhs) const
    {
        return !(*this == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, Rope const& r)
    {
        return os << r.to_string();
    }

private:
    class Node {
    public:
        using Handle = std::unique_ptr<Node>;

        Node(Handle l, Handle r)
            : left(std::move(l)), right(std::move(r)), weight(left->length())
        {
        }

        Node(std::string const& str)
            : left(nullptr), right(nullptr), weight(str.length()), fragment(str)
        {
        }

        Node(Node const& src)
            : left(nullptr), right(nullptr), weight(src.weight), fragment(src.fragment)
        {
            auto temp_l = src.left.get();
            if (temp_l != nullptr) {
                left = std::make_unique<Node>(*temp_l);
            }

            auto temp_r = src.right.get();
            if (temp_r != nullptr) {
                right = std::make_unique<Node>(*temp_r);
            }
        }

        size_t length() const
        {
            if (is_leaf()) {
                return weight;
            }

            auto tmp = (right == nullptr) ? 0 : right->length();
            return weight + tmp;
        }

        const char& at(size_t pos) const
        {
            auto w = weight;

            if (is_leaf()) {
#if 0 // @TODO(BML) \
    // if node is a leaf, return the character at the specified index
                if (pos >= weight) {
                    throw std::invalid_argument("rope index out of bounds");
                }
                else {
                    return fragment[pos];
                }
#else // the length was already checked so this should be fine
                return fragment[pos];
#endif
            }
            else {
                // else search the appropriate child node
                if (pos < w) {
                    return left->at(pos);
                }
                else {
                    return right->at(pos - w);
                }
            }
        }

        char& at(size_t pos)
        {
            auto w = weight;

            if (is_leaf()) {
#if 0 // @TODO(BML) \
    // if node is a leaf, return the character at the specified index
                if (pos >= weight) {
                    throw std::invalid_argument("rope index out of bounds");
                }
                else {
                    return fragment[pos];
                }
#else // the length was already checked so this should be fine
                return fragment[pos];
#endif
            }
            else {
                // else search the appropriate child node
                if (pos < w) {
                    return left->at(pos);
                }
                else {
                    return right->at(pos - w);
                }
            }
        }

        std::string substring(size_t start, size_t len) const
        {
            auto w = weight;
            if (is_leaf()) {
                if (len < w) {
                    return fragment.substr(start, len);
                }
                else {
                    return fragment;
                }
            }
            else {
                // check if start index in left subtree
                if (start < w) {
                    std::string lresult = (left == nullptr) ? std::string{ "" } : left->substring(start, len);
                    if ((start + len) > w) {
                        // get number of characters in left subtree
                        auto tmp = w - start;
                        std::string rresult = (right == nullptr) ? std::string{ "" } : right->substring(w, len - tmp);
                        return lresult.append(rresult);
                    }
                    else {
                        return lresult;
                    }
                }
                else {
                    // if start index is in the right subtree...
                    return (right == nullptr) ? std::string{ "" } : right->substring(start - w, len);
                }
            }
        }

        std::string tree_to_string() const
        {
            if (is_leaf()) {
                return fragment;
            }

            std::string lresult = (left == nullptr) ? std::string{ "" } : left->tree_to_string();
            std::string rresult = (right == nullptr) ? std::string{ "" } : right->tree_to_string();
            return lresult.append(rresult);
        }

        static std::pair<Handle, Handle> split(Handle node, size_t index)
        {
            auto w = node->weight;

            // if the given node is a leaf, split the leaf
            if (node->is_leaf()) {
                return std::make_pair(
                    std::make_unique<Node>(node->fragment.substr(0, index)),
                    std::make_unique<Node>(node->fragment.substr(index, w - index)));
            }

            // if the given node is a concat (internal) node, compare index to weight and handle accordingly
            Handle old_right = std::move(node->right);
            if (index < w) {
                node->right = nullptr;
                node->weight = index;
                auto splitLeftResult = Node::split(std::move(node->left), index);
                node->left = std::move(splitLeftResult.first);
                return std::make_pair(std::move(node), std::make_unique<Node>(std::move(splitLeftResult.second), std::move(old_right)));
            }
            else if (w < index) {
                auto splitRightResult = Node::split(std::move(old_right), index - w);
                node->right = std::move(splitRightResult.first);
                return std::make_pair(std::move(node), std::move(splitRightResult.second));
            }
            else {
                return std::make_pair(std::move(node->left), std::move(old_right));
            }
        }

        size_t depth() const
        {
            if (is_leaf())
                return 0;

            size_t lresult = (left == nullptr) ? 0 : left->depth();
            size_t rresult = (right == nullptr) ? 0 : right->depth();
            return std::max(++lresult, ++rresult);
        }

        void get_leaves(std::vector<Node*>& v)
        {
            if (is_leaf()) {
                v.push_back(this);
            }
            else {
                auto tmpLeft = left.get();
                if (tmpLeft != nullptr)
                    tmpLeft->get_leaves(v);

                auto tmpRight = right.get();
                if (tmpRight != nullptr)
                    tmpRight->get_leaves(v);
            }
        }

    private:
        bool is_leaf() const
        {
            return left == nullptr && right == nullptr;
        }

        Handle left;
        Handle right;
        size_t weight;
        std::string fragment;
    };

    Node::Handle root;

    std::array<int, 256> preprocess(std::string const& pattern)
    {
        std::array<int, 256> table;
        auto plen = static_cast<int>(pattern.length());
        for (auto& entry : table) {
            entry = plen;
        }

        for (int i = 0; i < plen - 1; ++i) {
            table[pattern[i]] = plen - 1 - i;
        }

        return table;
    }

    size_t fib(size_t n) const
    {
        if (n == 0)
            return 0;

        size_t a = 0, b = 1, next;
        for (size_t i = 2; i <= n; i++) {
            next = a + b;
            a = b;
            b = next;
        }

        return b;
    }

    std::vector<size_t> build_fib_list(size_t len)
    {
        // initialize a and b to the first and second fib numbers respectively
        size_t a = 0, b = 1, next;
        std::vector<size_t> intervals = std::vector<size_t>();
        while (a <= len) {
            if (a > 0) {
                intervals.push_back(b);
            }
            next = a + b;
            a = b;
            b = next;
        }
        return intervals;
    }
};
