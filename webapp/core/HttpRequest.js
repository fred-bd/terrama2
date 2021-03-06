var AbstractRequest = require('./AbstractRequest');
var Promise = require('bluebird');
var Exceptions = require("./Exceptions");
var NodeUtils = require('util');
var Requester = require('request');
var Form = require('./Enums').Form;
var FormField = Form.Field;
var UriPattern = require("./Enums").Uri;
var Utils = require("./Utils");

var HttpRequest = function(params) {
  AbstractRequest.apply(this, arguments);
};

HttpRequest.prototype = Object.create(AbstractRequest.prototype);
HttpRequest.prototype.constructor = HttpRequest;

HttpRequest.prototype.request = function() {
  var self = this;
  return  new Promise(function(resolve, reject) {
    Requester(self.uri, function(err, resp, body) {
      if (err)
        reject(new Exceptions.ConnectionError("Error in http request"));
      else {
        if (resp.statusCode === 200)
          resolve();
        else
          reject(new Exceptions.ConnectionError(NodeUtils.format("Error in http request: (%d) - %s", resp.statusCode, resp.statusMessage)));
      }
    });
  });
};

HttpRequest.fields = function() {
  var properties = {};
  properties[UriPattern.HOST] = {
    title: "Address",
    type: FormField.TEXT
  };

  properties[UriPattern.PORT] = {
    title: "Port",
    minimum: 2,
    maximum: 65535,
    type: FormField.NUMBER,
    default: 80
  };

  properties['timeout'] = {
    title: "Data server Timeout (sec)",
    minimum: 1,
    type: FormField.NUMBER,
    default: 8
  };

  properties[UriPattern.USER] = {
    title: "Username",
    type: FormField.TEXT
  };

  properties[UriPattern.PASSWORD] = {
    title: "Password",
    type: FormField.TEXT
  };

  properties[UriPattern.PATHNAME] = {
    title: "Path",
    type: FormField.TEXT
  };

  return {
    name: "HTTP",
    properties: properties,
    required: [UriPattern.HOST, UriPattern.PORT],
    display: [
      {
        key: UriPattern.HOST,
        type: FormField.TEXT,
        htmlClass: 'col-md-6 terrama2-schema-form'
      },
      {
        key: UriPattern.PORT,
        type: FormField.NUMBER,
        htmlClass: 'col-md-3 terrama2-schema-form'
      },
      {
        key:'timeout',
        type: FormField.NUMBER,
        htmlClass: 'col-md-3 terrama2-schema-form'
      },
      {
        key: UriPattern.USER,
        type: FormField.TEXT,
        htmlClass: 'col-md-6 terrama2-schema-form'
      },
      {
        key: UriPattern.PASSWORD,
        type: FormField.PASSWORD,
        htmlClass: 'col-md-6 terrama2-schema-form'
      },
      {
        key: UriPattern.PATHNAME,
        type: Form.Field.TEXT,
        htmlClass: 'col-md-12'
      }
    ]
  };
};



module.exports = HttpRequest;