# db

json store with url-based data structure indexing

for example

```
GET https://cs.p13i.io/db/UUID

200
{
  “someKey”: [
    { “id”: 1 },
    { “id”: 2,
      “someObject”:  {
        “something123”: false
      }
    },
  ]
}

GET https://cs.p13i.io/db/UUID/someKey/1/someObject/something123/

200
false

GET https://cs.p13i.io/db/UUID/someKey/0/

200
{ “id”: 1 }

GET https://cs.p13i.io/db/UUID/someKey/1/someObject/noKey

404
```
