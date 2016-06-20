(function ($) {
  /*
   * This is the c3js implementation class for tableCharts.
   *
   * It is called by the tableCharts class and assumes data has been parsed from
   * the table and all applicable settings and data are present.
   *
   * @see tableCharts default settings for structure and available settings.
   *
   * @param settings
   *   The parsed settings from tableCharts.
   */

  // All tableCharts chart implementations should be a method of this object.
  window.tableChartsChart = window.tableChartsChart || {};

  tableChartsChart.c3js = function (settings) {

    // Ensure library is loaded.
    if (typeof c3 === 'undefined') {
      alert('c3js library not found');
      return;
    }

    // Self instance.
    var self = this;

    // Store settings pased from parser.
    self.settings = settings;

    // Base options object that gets passed to c3js.
    self.options = {
      bindto: '#' + self.settings.chartDomId,
      color: {pattern: self.settings.palette},
      oninit: function () {
        self.postBuildCallback();
      },
      data: {},
      axis: {}
    };

    /*
     * Initialize the C3 Chart.
     */
    self.init = function () {
      // Parse all the settings.
      self
        .parseDataOptions()
        .parseAxisOptions()
        .parseGridOptions()
        .parsePointOptions()
        .parseBarOptions()
        .parseChartOptions();

      // Create chart.
      c3.generate(self.options);
    };

    /*
     * Parse data and data settings.
     */
    self.parseDataOptions = function () {
      var data = {};

      // Type of chart is stored in the data.
      data.type = self.settings.type;

      // Placeholder for the data columns.
      data.columns = [];

      // Stacked can be applied to most charts, the stack order used is
      // the column order.
      if (self.settings.stacked) {
        data.groups = [self.settings.group];
      }

      // Apply styles (currently only works with lines and dashes)
      if (self.settings.styles.length) {
        data.regions = {};
        $(self.settings.styles).each(function (i, d){
          data.regions[d.set] = [{style: d.style}];
        });
      }

      // Add X axis labels.
      if (self.settings.xLabels.length > 1) {
        data.x = 'x';
        data.columns.push(self.settings.xLabels);
      }

      // Show labels on data points?
      data.labels = self.settings.labels;

      // Add any overrides to graph types based on the column.
      data.types = self.settings.types;
      data.colors = self.settings.paletteOverride;

      // Add the data columns.
      $(self.settings.columns).each(function (i, col) {
        data.columns.push(col);
      });

      // Add to options.
      self.options.data = data;

      // Return self for chaining.
      return self;
    };

    /*
     * Parse the axis options.
     */
    self.parseAxisOptions = function () {
      // Define the axis settings.
      var axis = {
        rotated: self.settings.rotated,
        x: {label: {text: self.settings.xLabel}, tick: {}},
        y: {label: {text: self.settings.yLabel}, tick: {}}
      };

      // Define the tick rotation.
      if (self.settings.xTickRotate !== 0) {
        axis.x.tick.rotate = parseInt(self.settings.xTickRotate);
      }

      // Define the tick counts.
      if (self.settings.xTickCount) {
        axis.x.tick.count = parseInt(self.settings.xTickCount);
      }
      if (self.settings.yTickCount) {
        axis.y.tick.count = parseInt(self.settings.yTickCount);
      }

      // Define the tick label culling (max labels).
      if (self.settings.xTickCull !== false) {
        axis.x.tick.culling = {max: parseInt(self.settings.xTickCull)};
        // Round labels to whole numbers.
        axis.x.tick.format = function (x) {
          return Math.round(x);
        };
      }

      // Perform rounding on Y axis values.
      axis.y.tick.format = function (y) {
        var places = Math.pow(10, parseInt(self.settings.yRound));
        return Math.round(y * places) / places;
      };

      // Tick culling prevents this being a category axis.
      if (self.settings.xLabels.length > 1 && self.settings.xTickCull === false) {
        axis.x.type = 'category';
      }

      // X Axis tick centered.
      if (self.settings.xTickCentered) {
        axis.x.tick.centered = self.settings.xTickCentered;
      }

      // Set the label positions.
      if (self.settings.xAxisLabelPos) {
        axis.x.label.position = self.settings.xAxisLabelPos;
      }
      if (self.settings.yAxisLabelPos) {
        axis.y.label.position = self.settings.yAxisLabelPos;
      }

      // Add to options.
      self.options.axis = axis;

      // Return self for chaining.
      return self;
    };

    /*
     * Parse grid options.
     */
    self.parseGridOptions = function () {

      switch (self.settings.grid) {
        case 'xy':
          self.options.grid = {x: {show: true}, y: {show: true}};
          break;
        case 'x':
          self.options.grid = {x: {show: true}};
          break;
        case 'y':
          self.options.grid = {y: {show: true}};
          break;
      }

      // Return self for chaining.
      return self;
    };

    /*
     * Parse point options.
     */
    self.parsePointOptions = function () {
      var point = {};

      // Optionally hide points from line/spline charts.
      if (self.settings.hidePoints) {
        point.show = false;
      }

      // Optionally set the point size.
      if (self.settings.pointSize) {
        point.r = self.settings.pointSize;
      }

      self.options.point = point;

      // Return self for chaining.
      return self;
    };

    /*
     * Parse bar options.
     */
    self.parseBarOptions = function () {
      if (self.settings.type === 'bar') {

        // Provide a width ratio for bars.
        self.options.bar = {width: {ratio: self.settings.barWidth}};
      }

      // Return self for chaining.
      return self;
    };

    /*
     * Parse generic chart options.
     */
    self.parseChartOptions = function () {
      // Add optional title.
      if (self.settings.showTitle) {
        self.options.title = {text: self.settings.title};
      }

      // Return self for chaining.
      return self;
    };

    /*
     * Perform post chart creation tasks.
     */
    self.postBuildCallback = function () {
      // Apply an opacity class to the svg. See govcms_ckan_display.css.
      $('#' + self.settings.chartDomId + ' > svg').attr('class', 'tc-area-opacity-' + self.settings.areaOpacity);
      // Execute any callbacks passed from tableCharts.
      self.settings.chartInitCallback();
    };

    // Create the chart.
    self.init();

  };

})(jQuery);
