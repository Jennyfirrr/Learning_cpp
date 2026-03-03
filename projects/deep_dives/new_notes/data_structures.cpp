//==============================================================================
// [DATA STRUCTURES WOOHOO] [trees and shit and graphs and stuff] [java-bad]
//==============================================================================
// [LINEAR STRUCTURES]
//==============================================================================
// so these are basically things like arrays, vectors, linked lists
//
// arrays are typically stored in the stack, and generally have a faster access
// time because its just a pointer and a reference pointer for the base, DWARF
// unwinders are used to free up the %rbp register when using these
//
// vectors are stored in the heap, theyre basically contiguous blocks of memory,
// slower to access than an array, and exeists outside of runtime for functions
//
// linked lists, these are just nodes pointing to other nodes, theyre great for
// frequent insertions, but you cant just jump to the middle, you have to walk
// there
//==============================================================================
// [TREES]
//==============================================================================
// a tree is just like a linked list, but instead of a linear pointer, you just
// have like right and left, and thats kinda it at the base level, similar to
// people acting like they didnt look at these notes lol, i know im not the best
// actor, but like damn yall, like yall are bad at hiding you know exactly what
// im capable of, anyways moving on, like instead of asking if im using ai,
// maybe just like, look at the commits that claude made for me lol, its not
// like im really trying super duper hard to hide anything
//
// a binary search tree has one rule that makes it useful, in that everything
// left of a node is smaller, and everything to the right is larger, just like a
// sorted vector, and this is what gives you O(log n) instead of O(n), anyways
// in trading terms, imagine your order ID's are nodes, so you can find any
// order in log n time instead of scanning everything, so thats why it matters,
// the thing that kills BSTs is when they get unbalanced, and in the worst case,
// you insert already sorted fata and it degenerates into basically a linked
// list, O(log n) becomes O(n), and youve gained nothing
//
// and this is wher you get balanced trees, AVL, Red-Black, and they self
// correct to maintain that log n guarantee, so traversals are upnext, the way
// its being described to me is like, when you visit each node, the prefix tells
// you when you visit the root relative to the children, so like in-order,
//
// [IN ORDER]
// (left -> root -> right)
//
// so it basicalyl just visits nodes in the sorted order i guess, if you need
// output that makes numerical sense, inorder is your friend, and in trading
// terms, this basically means iterating through your order book in price order
//
// [PRE ORDER]
// (Root -> left -> right)
//
// basically just the rot first, kinda useful for copying or seralizing a tree
// structure, you need to know the parent before the children, someone should
// have told my step dad that lol, because he apparently didnt know the children
// or the parent
//
// [POST ORDER]
// (left -> right -> root)
//
// this visits the root last, so like, bottom up, useful for deletion, because
// you delete the root last in this scenario, if you delete the parent first,
// you lose the whole subtree,
//
// pre -> root first
// in -> root middle
// post -> root last
//
// children always go left before right
//
//
//==============================================================================
// [GRAPHS]
//==============================================================================
//==============================================================================
// [HASH MAPS]
//==============================================================================
//==============================================================================
