/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2024
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __CLINKLIST_H__
#define __CLINKLIST_H__

#include <malloc.h>

template <typename T> class CLinkList;

template <typename T>
class CLinkNode {
	friend class CLinkList<T>;

	public:
		CLinkNode();
		~CLinkNode();

		inline CLinkNode<T>* prev();
		inline CLinkNode<T>* next();
		inline T* data();

	private:
		CLinkNode<T>* _prev;
		CLinkNode<T>* _next;
		T* _data;
		int _del;
};

//--------------

template <typename T>
class CLinkList {
	public:
		CLinkList();
		~CLinkList();

		CLinkNode<T>* add(T*, int = 1);
		CLinkNode<T>* insert(T*, CLinkNode<T>*, int = 1);
		void del(CLinkNode<T>*);
		void empty();

		inline CLinkNode<T>* first();

		int num();

private:
		CLinkNode<T>* _first;
		int _num;

};

//==============

template<typename T>
CLinkNode<T>::CLinkNode() {
	this->_data = NULL;
	this->_prev = NULL;
	this->_next = NULL;
	this->_del = 1;
}

template <typename T>
CLinkNode<T>::~CLinkNode() {
	if (this->_del && this->_data) {
		if (this->_del == 1)
			delete this->_data;
		else if (this->_del == 2)
			free(this->_data);
	}
}

template <typename T>
CLinkNode<T>* CLinkNode<T>::prev() {
	return this->_prev;
}

template <typename T>
CLinkNode<T>* CLinkNode<T>::next() {
	return this->_next;
}

template <typename T>
T* CLinkNode<T>::data() {
	return this->_data;
}

//--------------

template <typename T>
CLinkList<T>::CLinkList() {
	this->_first = NULL;
	this->_num = 0;
}

template <typename T>
CLinkList<T>::~CLinkList() {
	this->empty();
}

template <typename T>
CLinkNode<T>* CLinkList<T>::add(T* data, int del) {
	CLinkNode<T>* p = this->_first;
	CLinkNode<T>* q = NULL;
	
	while (p) {
		q = p;
		p = p->_next;
	}
	
	p = new CLinkNode<T>;
	
	if (q)
		q->_next = p;
	else
		this->_first = p;

	p->_prev = q;
	p->_next = NULL;
	p->_data = data;
	p->_del = del;

	this->_num++;

	return p;
}

template <typename T>
CLinkNode<T>* CLinkList<T>::insert(T* data, CLinkNode<T>* after, int del) {
	CLinkNode<T>* p = this->_first;
	CLinkNode<T>* q = NULL;

	while (q != after) {
		q = p;
		p = p->_next;
	}
	
	CLinkNode<T>* r = new CLinkNode<T>;

	if (q)
		q->_next = r;
	else
		this->_first = r;

	if (p)
		p->_prev = r;

	r->_prev = q;
	r->_next = p;
	r->_data = data;
	r->_del = del;

	this->_num++;

	return r;
}

template <typename T>
void CLinkList<T>::del(CLinkNode<T>* node) {
	CLinkNode<T>* p = node->_prev;
	CLinkNode<T>* n = node->_next;

	if (p)
		p->_next = n;
	else
		this->_first = n;

	if (n) n->_prev = p;

	delete node;

	this->_num--;
}

template <typename T>
void CLinkList<T>::empty() {
	CLinkNode<T>* p = this->_first;
	CLinkNode<T>* q = NULL;

	while (p) {
		q = p->_next;
		delete p;
		p = q;
	}

	this->_first = NULL;
	this->_num = 0;
}

template <typename T>
CLinkNode<T>* CLinkList<T>::first() {
	return this->_first;
}

template <typename T>
int CLinkList<T>::num() {
	return this->_num;
}

#endif //__CLINKLIST_H__
