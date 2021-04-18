#pragma once
#include <memory>
#include <string>
#include <exception>
#include <stdexcept>


class LinkedList
{
	struct Node
	{
		Node(std::string const data, Node* pPrev, std::unique_ptr<Node>&& pNext)
			: m_data(data)
			, m_pPrev(pPrev)
			, m_pNext(std::move(pNext))
		{
		}

		std::string m_data;
		std::unique_ptr<Node> m_pNext;
		Node* m_pPrev;
	};

public:
		class CIterator
		{
			friend LinkedList;

			CIterator(Node* node, bool isReversed)
				: m_node(node)
				, m_isReversed(isReversed)
			{
			}


		public:
			CIterator() = default;

			bool IsReversed() const
			{
				return m_isReversed;
			}

			std::string& operator*() const
			{
				if (m_node)
				{
					return m_node->m_data;
				}

				throw std::runtime_error("Empty iterator cant be dereferenced\n");

			}

			CIterator& operator++()
			{
				if (!m_node)
				{
					throw std::runtime_error("Iterator cant be incremented");
				}

				if (IsReversed())
				{
					m_node = m_node->m_pPrev;
				}
				else
				{
					m_node = m_node->m_pNext.get();
				}

				return *this;
			}

			bool operator==(CIterator const& rightIterator)
			{
				return this->m_node == rightIterator.m_node;
			}

			bool operator!=(CIterator const& rightIterator)
			{
				return !(*this == rightIterator);
			}

		private:
			Node* m_node = nullptr;
			bool m_isReversed;
		};

		LinkedList::CIterator begin()
		{
			return CIterator(m_firstNode.get(), false);
		}

		LinkedList::CIterator end()
		{
			return CIterator(nullptr, false);
		}

	void Delete(CIterator& iterator)
	{
		if (!iterator.m_node)
		{
			throw std::runtime_error("Invalid element to be deleted");
		}

		if (iterator.m_node == m_lastNode)
		{
			if (m_lastNode->m_pPrev)
			{
				m_lastNode = iterator.m_node->m_pPrev;
				iterator.m_node->m_pPrev->m_pNext = nullptr;
			}
			else
			{
				m_firstNode = nullptr;
				m_lastNode = nullptr;
			}
		}
		else if (iterator.m_node == m_firstNode.get())
		{
			m_firstNode = std::move(iterator.m_node->m_pNext);
			m_firstNode->m_pPrev = nullptr;
		}
		else
		{
			iterator.m_node->m_pNext->m_pPrev = iterator.m_node->m_pPrev;
			iterator.m_node->m_pPrev->m_pNext = std::move(iterator.m_node->m_pNext);
		}
		m_size--;
		iterator.m_node = nullptr;
	}

	void Clear()
	{
		for (auto it = begin(); it != end(); it = begin())
		{
			Delete(it);
		}
	}


	void Push(std::string const& data)
	{
		auto newNode = std::make_unique<Node>(data, m_lastNode, nullptr);
		Node* newLastNode = newNode.get();

		if (m_lastNode)
		{
			m_lastNode->m_pNext = std::move(newNode);
		}
		else
		{
			m_firstNode = std::move(newNode);
		}

		m_lastNode = newLastNode;
		m_size++;
	}

	/*Node* Pop()
	{
		if (IsEmpty())
		{
			throw std::runtime_error("List is empty");
		}

		if (m_size > 1)
		{
			m_firstNode = std::move(m_firstNode->m_pNext);
			m_firstNode->m_pPrev = nullptr;
		}

		if (m_size == 1)
		{
			auto node = m_firstNode.get();
			m_size--;
			m_firstNode = nullptr;
			m_lastNode = nullptr;
			return node;
		}

		m_size--;
		return m_firstNode.get();
	}*/


	bool IsEmpty()
	{
		return m_size == 0;
	}


	size_t GetSize()
	{
		return m_size;
	}

private:
	std::unique_ptr<Node> m_firstNode;
	Node* m_lastNode = nullptr;
	size_t m_size = 0;

};
