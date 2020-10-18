## Range

### Type
```cpp
class RangeRandomEngineBase
{
protected:
    static inline std::mt19937 rdEngine{ std::random_device{}() };
};
template <typename T, typename StepType, typename ValueType = std::common_type_t<T, StepType>)
class Range : RangeRandomEngineBase
{
    /*...*/
public:
    using value_type = ValueType;
    /*...*/
}
```

### Constructor
```cpp
template<typename T, typename StepType>
Range(T start, T end, StepType step);
```
Construct a ``Range`` class representing [start, end). When incremented, the ``current`` value is incremented with ``step``. Note: **according to deduction rules, ``start`` and ``end`` must have the same type.**

#### Public Member functions
```cpp
value_type operator*() const;
```
Returns the ``current`` value.

```cpp
Range<T, StepType, ValueType> begin();
```
Returns ``*this`` unchanged.

```cpp
value_type end();
```
Returns the ``end`` value.

```cpp
auto steps() const;
```
Returns the number of ramaining steps to go in the current ``Range``.

```cpp
bool operator!=(Range rhs) const;           //1
bool operator!=(value_type value) const;    //2
```
1. Returns ``this->current`` =?= ``rhs.current``
2. Returns ``this->current`` =?= ``value``

```cpp
template<typename Num, typename = std::enable_if_t<std::is_arithmetic_v<Num>>>
bool operator==(Num number) const;
```
Returns whether ``this->current``<=``number``<=``this->end``, only instantiated when ``number`` is a number type.


```cpp
Range& operator++();                //1
Range& operator+=(unsigned steps);  //2
```
1. increment ``*this``
2. increment ``*this`` ``steps`` times

```cpp
value_type rand();                                      //1
template<typename Container>
void fillRand(Container& container);                    //2
template<typename Container>
void fillRand(Container& container, size_t count);      //3
template<typename InputIt>
void fillRand(InputIt begin, InputIt end);              //4

value_type randFast() const;                            //5
template<typename Container>
void fillRandFast(Container& container);                //6
template<typename Container>
void fillRandFast(Container& container, size_t count);  //7
template<typename InputIt>
void fillRandFast(InputIt begin, InputIt end);          //8
```
- 1-4 uses ``std::uniform_<T>_distribution`` where ``T`` is some numeric types depending on ``value_type``
  1. Returns a single random number within [current, end)
  2. Fill ``container`` with random numbers within [current, end)
  3. Fill ``container`` with ``count`` random numbers within [current, end), equivalent to:
        ```cpp
        fillRand(std::begin(container), std::begin(container) + count)
        ```
  4. Fill the range pointed by the iterators [begin, end) with random numbers within [current, end)
- 5-8 uses ``rand()`` from ``<stdlib>``, which have the same usage as 1-4

### Non Member functions
```cpp
friend std::ostream& operator<<(std::ostream& os, Range const& range);
```
Print range in the format of: ``[current,end]``
