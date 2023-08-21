curl -X POST -d '{
  "query": "query GetPlan($from: InputCoordinates!, $to: InputCoordinates!) {\n  plan(from: $from, to: $to) {\n     from\n{\n    lat\n    lon\n    }\n    }\n  }",
  "operationName": "GetPlan",
  "variables": {
    "date" : "12032001",
    "from": {
      "lat": 2,
      "lon": 3
    },
    "to": {
      "lat": 1,
      "lon": 1
    }
  }
}\n
\n' http://localhost:8080/graphql
