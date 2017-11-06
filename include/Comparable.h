#ifndef __COMPARABLE_H_PROTECTED__
#define __COMPARABLE_H_PROTECTED__

enum cmp_t{	CMP_MIN = -1, CMP_EQ = 0, CMP_MAX = 1};

template <typename KeyType>
class Comparable
{
public:
	Comparable(KeyType key);
	~Comparable();

	cmp_t Compare(KeyType key) const {
		return _key == key ? CMP_EQ : (_key < key ? CMP_MIN : CMP_MAX);
	}

	KeyType Key() const {
		return _key;
	}
private:
	KeyType _key;
};

template <typename KeyType>
Comparable<KeyType>::Comparable(KeyType key)
	:_key(key)
{
}

template <typename KeyType>
Comparable<KeyType>::~Comparable()
{
}
#endif // __COMPARABLE_H_PROTECTED__
