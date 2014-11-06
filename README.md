MiniSQL
=======

a database system design course project

#### Global Naming Convention
```cpp
const int BLOCK_SIZE = 4*1024;
```

#### File Name Convention

catalog table:
```
{table_name}_t.cata
```
catalog index:
```
{index_name}_i.cata
```
index created by user:
```
{table_name}_{index_name}.index
```
index on primary key:
```
#pri_{primary key}.index
```
db:
```
{table_name}.db
```
