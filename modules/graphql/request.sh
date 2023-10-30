curl -X POST -d '{
  "id":	"GetPlan",
  "query": "query GetPlan(\n  $fromPlace: String!\n  $toPlace: String!\n  $intermediatePlaces: [InputCoordinates!]\n  $numItineraries: Int!\n  $modes: [TransportMode!]\n  $date: String!\n  $time: String!\n  $walkReluctance: Float\n  $walkBoardCost: Int\n  $minTransferTime: Int\n  $walkSpeed: Float\n  $wheelchair: Boolean\n  $ticketTypes: [String]\n  $arriveBy: Boolean\n  $transferPenalty: Int\n  $bikeSpeed: Float\n  $optimize: OptimizeType\n  $itineraryFiltering: Float\n  $unpreferred: InputUnpreferred\n  $allowedVehicleRentalNetworks: [String]\n  $locale: String\n  $modeWeight: InputModeWeight\n) {\n  plan(fromPlace: $fromPlace, toPlace: $toPlace, intermediatePlaces: $intermediatePlaces, numItineraries: $numItineraries, transportModes: $modes, date: $date, time: $time, walkReluctance: $walkReluctance, walkBoardCost: $walkBoardCost, minTransferTime: $minTransferTime, walkSpeed: $walkSpeed, wheelchair: $wheelchair, allowedTicketTypes: $ticketTypes, arriveBy: $arriveBy, transferPenalty: $transferPenalty, bikeSpeed: $bikeSpeed, optimize: $optimize, itineraryFiltering: $itineraryFiltering, unpreferred: $unpreferred, allowedVehicleRentalNetworks: $allowedVehicleRentalNetworks, locale: $locale, modeWeight: $modeWeight) {\n  date \n itineraries{\n startTime\n duration\n legs{\n from {\n        stop {\n     vehicleMode\n      }\n       }\n     route {\n        id\n   mode\n        }\n  }\n }\n }\n  }",
  "operationName": "GetPlan",
  "variables": {
      "allowedVehicleRentalNetworks": [],
  		"arriveBy": false,
      "bikeSpeed": 5.55,
      "date" : "2023-10-30",
      "from": {
        "lat": 50.758075,
        "lon": 6.105464
      },
      "to": {
        "lat": 50.782213999999996,
        "lon": 6.07519
      },
      "fromPlace": "Dachsteinstraße 6, 71083 Herrenberg::50.758075,6.105464",
      "intermediatePlaces": [],
      "itineraryFiltering": 1.5,
      "locale": "de",
      "minTransferTime": 120,
      "modes": [
      	{
      		"mode": "FLEX",
      		"qualifier": "DIRECT"
      	},
  			{
    			"mode": "FLEX",
    			"qualifier": "ACCESS"
    		},
    		{
    			"mode": "FLEX",
    			"qualifier": "EGRESS"
    		},
    		{
    			"mode": "BUS"
    		},
    		{
    			"mode": "RAIL"
    		},
    		{
    			"mode": "SUBWAY"
    		},
    		{
    			"mode": "WALK"
    		}
    	],
    	"modeWeight": null,
    	"numItineraries": 5,
    	"optimize": "TRIANGLE",
    	"ticketTypes": null,
    	"time": "11:26:00",
    	"toPlace": "Grasiger Weg, 71083 Herrenberg::50.782213999999996,6.07519",
    	"transferPenalty": 0,
    	"unpreferred": null,
    	"walkBoardCost": 600,
    	"walkReluctance": 2,
    	"walkSpeed": 1.38,
    	"wheelchair": false
    }
}\n
\n' http://localhost:8080/index/graphql
