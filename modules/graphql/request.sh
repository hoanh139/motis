curl -X POST -d '{
  "query": "query GetPlan($from: InputCoordinates!, $to: InputCoordinates!) {\n  plan(from: $from, to: $to) {\n     date\n    }\n  }",
  "operationName": "GetPlan",
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
