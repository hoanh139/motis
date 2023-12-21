# Documentation for Graph QL
By Ha Hoang Anh Le

In this module graphql we create an api that can handle Graph QL request.
The following step will be carried out when a request come.
- Extract the needed information from the query and variables to create a reasonable request to Motis.
- Create structures in c++ that resemble the syntax in `schema.otp.graphql` file.
- Use the information from motis response to create objects of these structs that match what is required in the query.
- Use the data type that cppgraphqlgen created to wrap the corresponding struct object.
- Send a response back to client.
