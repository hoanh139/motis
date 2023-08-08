curl -X POST -d '{
  "query": "Trip\n{ departure, arrival\n}",
  "variables": {
    "from": {
      "latitude": 0.0,
      "longitude": 0.0
    },
    "to": {
      "latitude": 0.0,
      "longitude": 0.0
    }
  }
}' http://localhost:8080/graphql
