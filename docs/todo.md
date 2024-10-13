## TODO

### Add support for file IO
- Add file type
- File functions
  - fopen
  - fclose
  - fflush
  - fread
  - fwrite
  - fget
  - fput
  - fgetline
  - ftell
  - fseek
- File variables
  - stdin
  - stdout
  - stderr

### Implement built-in data structures:
- List
  - **list**: make list
  - **lislen**: list length
  - **lisfront**: list front
  - **lisback**: list back
  - **lispushf**: list push front
  - **lispopf**: list pop front
  - **lispushb**: list push back
  - **lispopb**: list pop back
  - **lisget**: list get item at index
  - **lisset**: list set item at index
  - **lispush**: list push
  - **lispop**: list pop
  - **lisins**: list insert at index
  - **lisrem**: list remove index
  - **lisclr**: list clear
  - **lisfind**: list find value index
  - **liscount**: list count value
  - **lissort**: list sort
  - **lisrev**: list reverse
- Ordered table
  - **orderedmap**: make orderedmap
  - **omlen**: orderedmap length
  - **omhas**: orderedmap has key
  - **omget**: orderedmap get
  - **omset**: orderedmap set
  - **omrem**: orderedmap remove
  - **omclr**: orderedmap clear
  - **omfind**: orderedmap find by value
  - **omcount**: orderedmap count value
  - **omkeys**: orderedmap keys
  - **omvals**: orderedmap values
  - **omitems**: orderedmap items

### Add generic functions
- Add generic get function with support for instances, strings, arrays and hashmaps
  ```
  (get <instance|container> <index|key>) -> <value>
  ```
- Add generic set function with support for instances, strings, arrays and hashmaps
  ```
  (set <instance|container> <index|key> <value>) -> <value>
  ```
- Add generic find function with support for strings, arrays and hashmaps
  ```
  (find <container> <item>) -> <index|key>
  ```
- Add generic count function with support for strings, arrays and hashmaps
  ```
  (count <container> <item>) -> int
  ```
- Add generic sort function with support for strings and arrays
  ```
  (sort <sequence> key=null asc=true) -> <container>
  ```
- Add generic reverse function with support for strings and arrays
  ```
  (reverse <sequence>) -> <sequence>
  ```
- Add generic sum function with support for arrays and ranges
  ```
  (sum <sequence|range> [<start> [<end>]]) -> int|real
  ```
Note: Should work with future types; list and orderedmap.
