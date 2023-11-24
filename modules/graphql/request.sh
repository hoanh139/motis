curl -X POST -d '{
  "id":	"GetPlan",
  "query": "query GetPlan(\n  $fromPlace: String!\n  $toPlace: String!\n  $intermediatePlaces: [InputCoordinates!]\n  $numItineraries: Int!\n  $modes: [TransportMode!]\n  $date: String!\n  $time: String!\n  $walkReluctance: Float\n  $walkBoardCost: Int\n  $minTransferTime: Int\n  $walkSpeed: Float\n  $wheelchair: Boolean\n  $ticketTypes: [String]\n  $arriveBy: Boolean\n  $transferPenalty: Int\n  $bikeSpeed: Float\n  $optimize: OptimizeType\n  $itineraryFiltering: Float\n  $unpreferred: InputUnpreferred\n  $allowedVehicleRentalNetworks: [String]\n  $locale: String\n  $modeWeight: InputModeWeight\n) {\n ...SummaryPage_viewer_2MYIzG\n }\n\n fragment SummaryPage_viewer_2MYIzG on QueryType {\n plan(fromPlace: $fromPlace, toPlace: $toPlace, intermediatePlaces: $intermediatePlaces, numItineraries: $numItineraries, transportModes: $modes, date: $date, time: $time, walkReluctance: $walkReluctance, walkBoardCost: $walkBoardCost, minTransferTime: $minTransferTime, walkSpeed: $walkSpeed, wheelchair: $wheelchair, allowedTicketTypes: $ticketTypes, arriveBy: $arriveBy, transferPenalty: $transferPenalty, bikeSpeed: $bikeSpeed, optimize: $optimize, itineraryFiltering: $itineraryFiltering, unpreferred: $unpreferred, allowedVehicleRentalNetworks: $allowedVehicleRentalNetworks, locale: $locale, modeWeight: $modeWeight) {\n itineraries {\n duration\n ...ItineraryTab_itinerary\n }\n  }\n\n }\n\n fragment ItineraryTab_plan on Plan {\n  date\n}\n\n fragment ItineraryTab_itinerary on Itinerary {\n   duration\n  fares {\n    cents\n    components {\n      cents\n      fareId\n      }\n    type\n  }\n  legs {\n      from {\n      bikePark {\n        bikeParkId\n        name\n        id\n      }\n      bikeRentalStation {\n        networks\n        bikesAvailable\n        lat\n        lon\n        stationId\n        id\n      }\n      stop {\n     vehicleMode\n  alerts {\n          alertSeverityLevel\n          effectiveEndDate\n          effectiveStartDate\n          alertHeaderText\n          alertHeaderTextTranslations {\n            text\n            language\n          }\n          alertDescriptionText\n          alertDescriptionTextTranslations {\n            text\n            language\n          }\n          alertUrl\n          alertUrlTranslations {\n            text\n            language\n          }\n          id\n        }\n        }\n    }\n    to {\n    bikeRentalStation {\n        lat\n        lon\n        stationId\n        networks\n        bikesAvailable\n        id\n      }\n      stop {\n   vehicleMode\n        alerts {\n          alertSeverityLevel\n          effectiveEndDate\n       effectiveStartDate\n          alertHeaderText\n          alertHeaderTextTranslations {\n            text\n            language\n          }\n          alertDescriptionText\n          alertDescriptionTextTranslations {\n            text\n            language\n          }\n          alertUrl\n          alertUrlTranslations {\n            text\n            language\n          }\n        }\n       }\n      bikePark {\n        bikeParkId\n        name\n        id\n      }\n      carPark {\n        carParkId\n        name\n        id\n      }\n    }\n    dropOffBookingInfo {\n      message\n      dropOffMessage\n      contactInfo {\n        phoneNumber\n        infoUrl\n        bookingUrl\n      }\n    }\n      duration\n     route {\n      alerts {\n        alertSeverityLevel\n        effectiveEndDate\n        effectiveStartDate\n        entities {\n          __typename\n          ... on Route {\n            patterns {\n              code\n              id\n            }\n          }\n          ... on Node {\n            __isNode: __typename\n            id\n          }\n        }\n        alertHeaderText\n        alertHeaderTextTranslations {\n          text\n          language\n        }\n        alertDescriptionText\n        alertDescriptionTextTranslations {\n          text\n          language\n        }\n        alertUrl\n        alertUrlTranslations {\n          text\n          language\n        }\n        id\n      }\n     }\n    trip {\n       stoptimesForDate {\n        headsign\n        pickupType\n        realtimeState\n    }\n        }\n  }\n}\n\n",
  "operationName": "GetPlan",
  "variables": {
    "allowedVehicleRentalNetworks": [],
		"arriveBy": false,
    "bikeSpeed": 5.55,
    "date" : "2023-11-24",
    "from": {
      "lat": 50.77796,
      "lon": 6.072172
    },
    "to": {
      "lat": 50.772912999999996,
      "lon": 6.0971079999999995
    },
    "fromPlace": "Adalbertsteinweg, 52066 Aachen::50.780919999999995,6.073154",
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
  	"time": "11:45:00",
  	"toPlace": "Lothringerstraße, 52070 Aachen::50.772912999999996,6.0971079999999995",
  	"transferPenalty": 0,
  	"unpreferred": null,
  	"walkBoardCost": 600,
  	"walkReluctance": 2,
  	"walkSpeed": 1.38,
  	"wheelchair": false
  }
}\n
\n' http://localhost:8080/index/graphql
