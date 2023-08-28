curl -X POST -d '{
  "query": "query GetPlan($date: String!, $time: String!,$from: InputCoordinates!, $to: InputCoordinates!, $numItineraries: Int!) {\n  plan(date: $date, time: $time, from: $from, to: $to, numItineraries: $numItineraries) {\n     from\n{\n    lat\n    lon\n    }\n    }\n  }",
  "operationName": "GetPlan",
  "variables": {
    "date" : "2023-08-28",
    "time": "10:18:43",
    "from": {
      "lat": 2,
      "lon": 3
    },
    "to": {
      "lat": 1,
      "lon": 1
    },
    "numItineraries": 5
  }
}\n
\n' http://localhost:8080/graphql
