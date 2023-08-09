curl -X POST -d '{
  "query": "query GetTrip ($from: Location!, $to: Location!) {\n  trip(from: $from, to: $to) {\n      departure,\n      arrival\n    }\n  }",
  "operationName": "GetTrip",
  "variables": {
    "from": {
      "lat": 1,
      "lon": 1
    },
    "to": {
      "lat": 1,
      "lon": 1
    }
  }
}\n
\n' http://localhost:8080/graphql
