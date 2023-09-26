curl -X POST -d '{
  "query": "query GetPlan($date: String!, $time: String!, $fromPlace: String!, $toPlace: String!, $numItineraries: Int!) {\n  plan(date: $date, time: $time, fromPlace: $fromPlace, toPlace: $toPlace, numItineraries: $numItineraries) {\n  date \n itineraries{\n startTime\n duration\n legs{\n from {\n      name\n      lat\n      lon\n      stop {\n        gtfsId\n        zoneId\n        platformCode\n   id\n      }\n      bikeRentalStation {\n        bikesAvailable\n        networks\n        id\n      }\n    }\n     route {\n          gtfsId\n          type\n          shortName\n          id\n        }\n   agency {\n          gtfsId\n          fareUrl\n          name\n          id\n        }\n   }\n }\n }\n  }",
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
    "fromPlace": "Dachsteinstraße 6, 71083 Herrenberg::50.758075,6.105464",
    "toPlace": "Grasiger Weg, 71083 Herrenberg::50.782213999999996,6.07519",
    "numItineraries": 5
  }
}\n
\n' http://localhost:8080/graphql
