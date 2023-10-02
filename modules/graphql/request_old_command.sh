curl -X POST -d '{
  "query": "query GetPlan(\n  $fromPlace: String!\n  $toPlace: String!\n  $intermediatePlaces: [InputCoordinates!]\n  $numItineraries: Int!\n  $modes: [TransportMode!]\n  $date: String!\n  $time: String!\n  $walkReluctance: Float\n  $walkBoardCost: Int\n  $minTransferTime: Int\n  $walkSpeed: Float\n  $wheelchair: Boolean\n  $ticketTypes: [String]\n  $arriveBy: Boolean\n  $transferPenalty: Int\n  $bikeSpeed: Float\n  $optimize: OptimizeType\n  $itineraryFiltering: Float\n  $unpreferred: InputUnpreferred\n  $allowedVehicleRentalNetworks: [String]\n  $locale: String\n  $modeWeight: InputModeWeight\n) {\n  viewer {\n    ...SummaryPage_viewer_2MYIzG\n  }\n  serviceTimeRange {\n    ...SummaryPage_serviceTimeRange\n  }\n}\n\nfragment ItineraryLine_legs on Leg {\n  mode\n  rentedBike\n  startTime\n  endTime\n  distance\n  legGeometry {\n    points\n  }\n  transitLeg\n  interlineWithPreviousLeg\n  route {\n    shortName\n    color\n    type\n    agency {\n      name\n      id\n    }\n    id\n  }\n  from {\n    lat\n    lon\n    name\n    vertexType\n    bikeRentalStation {\n      lat\n      lon\n      stationId\n      networks\n      bikesAvailable\n      id\n    }\n    stop {\n      gtfsId\n      code\n      platformCode\n      id\n    }\n  }\n  to {\n    lat\n    lon\n    name\n    vertexType\n    bikeRentalStation {\n      lat\n      lon\n      stationId\n      networks\n      bikesAvailable\n      id\n    }\n    stop {\n      gtfsId\n      code\n      platformCode\n      id\n    }\n  }\n  trip {\n    gtfsId\n    stoptimes {\n      stop {\n        gtfsId\n        id\n      }\n      pickupType\n    }\n    id\n  }\n  intermediatePlaces {\n    arrivalTime\n    stop {\n      gtfsId\n      lat\n      lon\n      name\n      code\n      platformCode\n      id\n    }\n  }\n}\n\nfragment ItinerarySummaryListContainer_itineraries on Itinerary {\n  walkDistance\n  startTime\n  endTime\n  legs {\n    realTime\n    departureDelay\n    realtimeState\n    transitLeg\n    startTime\n    endTime\n    mode\n    distance\n    duration\n    rentedBike\n    interlineWithPreviousLeg\n    intermediatePlace\n    intermediatePlaces {\n      stop {\n        zoneId\n        id\n      }\n    }\n    route {\n      mode\n      shortName\n      type\n      color\n      agency {\n        name\n        id\n      }\n      alerts {\n        alertSeverityLevel\n        effectiveEndDate\n        effectiveStartDate\n        entities {\n          __typename\n          ... on Route {\n            patterns {\n              code\n              id\n            }\n          }\n          ... on Node {\n            __isNode: __typename\n            id\n          }\n        }\n        id\n      }\n      id\n    }\n    trip {\n      pattern {\n        code\n        id\n      }\n      stoptimes {\n        realtimeState\n        stop {\n          gtfsId\n          id\n        }\n        pickupType\n      }\n      alerts {\n        alertSeverityLevel\n        effectiveEndDate\n        effectiveStartDate\n        id\n      }\n      id\n    }\n    from {\n      name\n      lat\n      lon\n      stop {\n        gtfsId\n        zoneId\n        platformCode\n        alerts {\n          alertSeverityLevel\n          effectiveEndDate\n          effectiveStartDate\n          id\n        }\n        id\n      }\n      bikeRentalStation {\n        bikesAvailable\n        networks\n        id\n      }\n    }\n    to {\n      stop {\n        gtfsId\n        zoneId\n        alerts {\n          alertSeverityLevel\n          effectiveEndDate\n          effectiveStartDate\n          id\n        }\n        id\n      }\n      bikePark {\n        bikeParkId\n        name\n        id\n      }\n      carPark {\n        carParkId\n        name\n        id\n      }\n    }\n  }\n}\n\nfragment ItineraryTab_itinerary on Itinerary {\n  walkDistance\n  duration\n  startTime\n  endTime\n  arrivedAtDestinationWithRentedBicycle\n  fares {\n    cents\n    components {\n      cents\n      fareId\n      routes {\n        agency {\n          gtfsId\n          fareUrl\n          name\n          id\n        }\n        gtfsId\n        id\n      }\n    }\n    type\n  }\n  legs {\n    mode\n    ...LegAgencyInfo_leg\n    from {\n      lat\n      lon\n      name\n      vertexType\n      bikePark {\n        bikeParkId\n        name\n        id\n      }\n      bikeRentalStation {\n        networks\n        bikesAvailable\n        lat\n        lon\n        stationId\n        id\n      }\n      stop {\n        gtfsId\n        code\n        platformCode\n        vehicleMode\n        zoneId\n        alerts {\n          alertSeverityLevel\n          effectiveEndDate\n          effectiveStartDate\n          alertHeaderText\n          alertHeaderTextTranslations {\n            text\n            language\n          }\n          alertDescriptionText\n          alertDescriptionTextTranslations {\n            text\n            language\n          }\n          alertUrl\n          alertUrlTranslations {\n            text\n            language\n          }\n          id\n        }\n        id\n      }\n    }\n    to {\n      lat\n      lon\n      name\n      vertexType\n      bikeRentalStation {\n        lat\n        lon\n        stationId\n        networks\n        bikesAvailable\n        id\n      }\n      stop {\n        gtfsId\n        code\n        platformCode\n        zoneId\n        name\n        vehicleMode\n        alerts {\n          alertSeverityLevel\n          effectiveEndDate\n          effectiveStartDate\n          alertHeaderText\n          alertHeaderTextTranslations {\n            text\n            language\n          }\n          alertDescriptionText\n          alertDescriptionTextTranslations {\n            text\n            language\n          }\n          alertUrl\n          alertUrlTranslations {\n            text\n            language\n          }\n          id\n        }\n        id\n      }\n      bikePark {\n        bikeParkId\n        name\n        id\n      }\n      carPark {\n        carParkId\n        name\n        id\n      }\n    }\n    dropOffBookingInfo {\n      message\n      dropOffMessage\n      contactInfo {\n        phoneNumber\n        infoUrl\n        bookingUrl\n      }\n    }\n    legGeometry {\n      length\n      points\n    }\n    intermediatePlaces {\n      arrivalTime\n      stop {\n        gtfsId\n        lat\n        lon\n        name\n        code\n        platformCode\n        zoneId\n        id\n      }\n    }\n    realTime\n    realtimeState\n    transitLeg\n    rentedBike\n    startTime\n    endTime\n    departureDelay\n    arrivalDelay\n    interlineWithPreviousLeg\n    distance\n    duration\n    intermediatePlace\n    route {\n      shortName\n      color\n      gtfsId\n      type\n      longName\n      url\n      desc\n      agency {\n        gtfsId\n        fareUrl\n        name\n        phone\n        id\n      }\n      alerts {\n        alertSeverityLevel\n        effectiveEndDate\n        effectiveStartDate\n        entities {\n          __typename\n          ... on Route {\n            patterns {\n              code\n              id\n            }\n          }\n          ... on Node {\n            __isNode: __typename\n            id\n          }\n        }\n        alertHeaderText\n        alertHeaderTextTranslations {\n          text\n          language\n        }\n        alertDescriptionText\n        alertDescriptionTextTranslations {\n          text\n          language\n        }\n        alertUrl\n        alertUrlTranslations {\n          text\n          language\n        }\n        id\n      }\n      id\n    }\n    trip {\n      gtfsId\n      tripHeadsign\n      pattern {\n        code\n        id\n      }\n      stoptimesForDate {\n        headsign\n        pickupType\n        realtimeState\n        stop {\n          gtfsId\n          id\n        }\n      }\n      id\n    }\n  }\n}\n\nfragment ItineraryTab_plan on Plan {\n  date\n}\n\nfragment LegAgencyInfo_leg on Leg {\n  agency {\n    name\n    url\n    fareUrl\n    id\n  }\n}\n\nfragment RouteLine_pattern on Pattern {\n  code\n  geometry {\n    lat\n    lon\n  }\n  route {\n    mode\n    type\n    color\n    id\n  }\n  stops {\n    lat\n    lon\n    name\n    gtfsId\n    platformCode\n    code\n    ...StopCardHeaderContainer_stop\n    id\n  }\n}\n\nfragment StopCardHeaderContainer_stop on Stop {\n  gtfsId\n  name\n  code\n  desc\n  zoneId\n  alerts {\n    alertSeverityLevel\n    effectiveEndDate\n    effectiveStartDate\n    id\n  }\n  lat\n  lon\n  stops {\n    name\n    desc\n    id\n  }\n}\n\nfragment SummaryPage_serviceTimeRange on serviceTimeRange {\n  start\n  end\n}\n\nfragment SummaryPage_viewer_2MYIzG on QueryType {\n  plan(fromPlace: $fromPlace, toPlace: $toPlace, intermediatePlaces: $intermediatePlaces, numItineraries: $numItineraries, transportModes: $modes, date: $date, time: $time, walkReluctance: $walkReluctance, walkBoardCost: $walkBoardCost, minTransferTime: $minTransferTime, walkSpeed: $walkSpeed, wheelchair: $wheelchair, allowedTicketTypes: $ticketTypes, arriveBy: $arriveBy, transferPenalty: $transferPenalty, bikeSpeed: $bikeSpeed, optimize: $optimize, itineraryFiltering: $itineraryFiltering, unpreferred: $unpreferred, allowedVehicleRentalNetworks: $allowedVehicleRentalNetworks, locale: $locale, modeWeight: $modeWeight) {\n    ...SummaryPlanContainer_plan\n    ...ItineraryTab_plan\n    itineraries {\n      duration\n      startTime\n      endTime\n      ...ItineraryTab_itinerary\n      ...SummaryPlanContainer_itineraries\n      legs {\n        mode\n        ...ItineraryLine_legs\n        transitLeg\n        legGeometry {\n          points\n        }\n        route {\n          gtfsId\n          type\n          shortName\n          id\n        }\n        trip {\n          gtfsId\n          directionId\n          stoptimesForDate {\n            scheduledDeparture\n            pickupType\n          }\n          pattern {\n            ...RouteLine_pattern\n            id\n          }\n          id\n        }\n        from {\n          name\n          lat\n          lon\n          stop {\n            gtfsId\n            zoneId\n            id\n          }\n          bikeRentalStation {\n            bikesAvailable\n            networks\n            id\n          }\n        }\n        to {\n          stop {\n            gtfsId\n            zoneId\n            id\n          }\n          bikePark {\n            bikeParkId\n            name\n            id\n          }\n        }\n      }\n    }\n  }\n}\n\nfragment SummaryPlanContainer_itineraries on Itinerary {\n  ...ItinerarySummaryListContainer_itineraries\n  endTime\n  startTime\n  legs {\n    mode\n    to {\n      bikePark {\n        bikeParkId\n        name\n        id\n      }\n    }\n    ...ItineraryLine_legs\n    transitLeg\n    legGeometry {\n      points\n    }\n    route {\n      gtfsId\n      id\n    }\n    trip {\n      gtfsId\n      directionId\n      stoptimesForDate {\n        scheduledDeparture\n      }\n      pattern {\n        ...RouteLine_pattern\n        id\n      }\n      id\n    }\n  }\n}\n\nfragment SummaryPlanContainer_plan on Plan {\n  date\n  itineraries {\n    startTime\n    endTime\n    legs {\n      mode\n      ...ItineraryLine_legs\n      transitLeg\n      legGeometry {\n        points\n      }\n      route {\n        gtfsId\n        id\n      }\n      trip {\n        gtfsId\n        directionId\n        stoptimesForDate {\n          scheduledDeparture\n          pickupType\n        }\n        pattern {\n          ...RouteLine_pattern\n          id\n        }\n        id\n      }\n      from {\n        name\n        lat\n        lon\n        stop {\n          gtfsId\n          zoneId\n          id\n        }\n        bikeRentalStation {\n          bikesAvailable\n          networks\n          id\n        }\n      }\n      to {\n        stop {\n          gtfsId\n          zoneId\n          id\n        }\n        bikePark {\n          bikeParkId\n          name\n          id\n        }\n      }\n    }\n  }\n}\n",
  "operationName": "GetPlan",
  "variables": {
    "allowedVehicleRentalNetworks": [],
		"arriveBy": false,
    "bikeSpeed": 5.55,
    "date" : "2023-08-24",
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
\n' http://localhost:8080/graphql

curl -X POST -d '{
  "query": "query GetPlan($date: String!, $time: String!, $fromPlace: String!, $toPlace: String!, $numItineraries: Int!) {\n  plan(date: $date, time: $time, fromPlace: $fromPlace, toPlace: $toPlace, numItineraries: $numItineraries) {\n  date \n itineraries{\n startTime\n duration\n legs{\n from {\n      name\n      lat\n      lon\n      stop {\n        gtfsId\n        zoneId\n        platformCode\n   id\n      }\n      bikeRentalStation {\n        bikesAvailable\n        networks\n        id\n      }\n    }\n     route {\n          gtfsId\n          type\n          shortName\n          id\n        }\n  legGeometry {\n    points\n  }\n  agency {\n          gtfsId\n          fareUrl\n          name\n          id\n        }\n   }\n }\n }\n  }",
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

