"use strict";

/**
 * Controller responsible for returning the attributes table data accordingly with the received parameters.
 * @class CheckGridFileController
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberHttp - 'http' module.
 * @property {object} memberFs - 'fs' module.
 * @property {object} memberPath - 'path' module.
 * @property {object} memberAdminHostInfo - WebAdmin host info.
 */
var CheckGridFileController = function(app) {

  // 'http' module
  var memberHttp = require('http');
  // 'fs' module
  var memberFs = require('fs');
  // 'path' module
  var memberPath = require('path');
  // WebAdmin host info
  var memberAdminHostInfo = JSON.parse(memberFs.readFileSync(memberPath.join(__dirname, "../config/monitor.json"), "utf-8")).webadmin;

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function checkGridFile
   * @memberof CheckGridFileController
   * @inner
   */
  var checkGridFile = function(request, response) {
    var url = memberAdminHostInfo.protocol + memberAdminHostInfo.host + ":" + memberAdminHostInfo.port + memberAdminHostInfo.basePath + "check-grid?dpi=" + layer.exportation.dataProviderId + "&mask=" + layer.exportation.mask + "&file=" + layer.name;

    if(layer.dateInfo.dates !== undefined && layer.dateInfo.dates.length > 0)
      url += "." + layer.dateInfo.dates[layer.dateInfo.initialDateIndex] + "&date=" + layer.dateInfo.dates[layer.dateInfo.initialDateIndex];

    memberHttp.get(url, function(resp) {
      var body = '';

      resp.on('data', function(chunk) {
        body += chunk;
      });

      resp.on('end', function() {
        try {
          body = JSON.parse(body);
        } catch(ex) {
          body = { result: false };
        }

        // JSON response
        response.json(body);
      });
    }).on("error", function(e) {
      console.error(e.message);
      response.json({ result: false });
    });
  };

  return checkGridFile;
};

module.exports = CheckGridFileController;