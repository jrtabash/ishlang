## TODO

### Additional File IO Support
  - withfile (fopen, do work, fclose)
  - Examples
    ```
    (withfile f (fopen "some/path.txt" 'w')
      (fwriteln f "some text"))

    (withfile f (fopen "some/path.txt" 'r')
      (foreach line f
        (println line)))
    ```

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
  - Add list support to generic functions
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
  - Add orderedmap support to generic functions
