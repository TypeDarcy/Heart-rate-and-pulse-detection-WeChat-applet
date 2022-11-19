// pages/show/show.js
import * as echarts from '../../ec-canvas/echarts';
var util = require('../../utils/util.js');
const app = getApp();
let chart;

function TempChart(canvas, width, height, dpr) {
   chart = echarts.init(canvas, null, {
    width: width,
    height: height,
    devicePixelRatio: dpr // new
  });
  canvas.setChart(chart);

  var option = {
    title: {
      text: '体温',
      left: 'center'
    },
    grid: {
      containLabel: true
    },
    tooltip: {
      show: true,
      trigger: 'axis'
    },
    xAxis: {
      type: 'category',
      boundaryGap: false,
      data:null
      // show: false
    },
    yAxis: {
      x: 'center',
      min: 35,
      type: 'value',
      splitLine: {
        lineStyle: {
          type: 'dashed'
        }
      }
      // show: false
    },
    series: [{
      name: 'Temp',
      type: 'line',
      smooth: true,
      data:null
    }]
  };
  setInterval(function() {
    option.series[0].data = app.globalData.Templist;
    option.xAxis.data = app.globalData.timelist;
    console.log(app.globalData.timelist)
    chart.setOption(option, true);
  }, 2000)
  chart.setOption(option);
  return chart;
}
function Sao2Chart(canvas, width, height, dpr) {
  const chart = echarts.init(canvas, null, {
    width: width,
    height: height,
    devicePixelRatio: dpr // new
  });
  canvas.setChart(chart);

  var option = {
    title: {
      text: '血氧',
      left: 'center'
    },
    grid: {
      containLabel: true
    },
    tooltip: {
      show: true,
      trigger: 'axis'
    },
    xAxis: {
      type: 'category',
      boundaryGap: false,
      data: null
      // show: false
    },
    yAxis: {
      x: 'center',
      type: 'value',
      min: 60,
      splitLine: {
        lineStyle: {
          type: 'dashed'
        }
      }
      // show: false
    },
    series: [{
      name: 'Sao2',
      type: 'line',
      smooth: true,
      data: null
    }]
  };
  setInterval(function() {
    option.series[0].data = app.globalData.Sao2list;
    option.xAxis.data = app.globalData.timelist;
    chart.setOption(option, true);
  }, 2000)
  chart.setOption(option);
  return chart;
}
function HrChart(canvas, width, height, dpr) {
  const chart = echarts.init(canvas, null, {
    width: width,
    height: height,
    devicePixelRatio: dpr // new
  });
  canvas.setChart(chart);

  var option = {
    title: {
      text: '心率',
      left: 'center'
    },
    grid: {
      containLabel: true
    },
    tooltip: {
      show: true,
      trigger: 'axis'
    },
    xAxis: {
      type: 'category',
      boundaryGap: false,
      data:null
      // show: false
    },
    yAxis: {
      x: 'center',
      type: 'value',
      min: 45,
      splitLine: {
        lineStyle: {
          type: 'dashed'
        }
      }
      // show: false
    },
    series: [{
      name: 'Hr',
      type: 'line',
      smooth: true,
      data:null
    }]
  };
  setInterval(function() {
    option.series[0].data = app.globalData.Hrlist;
    option.xAxis.data = app.globalData.timelist;
    chart.setOption(option, true);
  }, 2000)
  chart.setOption(option);
  return chart;
}

Page({

  /**
   * 页面的初始数据
   */
  data: {
    
    ecTemp: {
      onInit: TempChart
    },
    ecSao2: {
      onInit: Sao2Chart
    },
    ecHr: {
      onInit: HrChart
    }
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {

  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady() {
    
  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow() {
  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide() {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload() {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh() {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom() {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage() {

  }
})

