MiniSQL
=======

a database system design course project

#### Global Naming Convention
```cpp
const int BLOCK_SIZE = 4*1024;
```

#### File Name Convention

catalog table:
```cpp
{table_name}_t.cata
```
catalog index:
```cpp
{index_name}_i.cata
```
index created by user:
```cpp
{table_name}_{index_name}.index
```
index on primary key:
```cpp
#pri_{table_name}_{primary key}.index
```
db:
```cpp
{table_name}.db
```
