curl -X POST -d '{
  "query": "query GetPlan($date: String!, $time: String!,$from: InputCoordinates!, $to: InputCoordinates!, $numItineraries: Int!) {\n  plan(date: $date, time: $time, from: $from, to: $to, numItineraries: $numItineraries) {\n     from\n{\n    lat\n    lon\n    }\n    }\n  }",
  "operationName": "GetPlan",
  "variables": {
    "date" : "2023-08-24",
    "time": "11:26:00",
    "from": {
      "lat": 50.758075,
      "lon": 6.105464
    },
    "to": {
      "lat": 50.782213999999996,
      "lon": 6.07519
    },
    "numItineraries": 5
  }
}\n
\n' http://localhost:8080/graphql
