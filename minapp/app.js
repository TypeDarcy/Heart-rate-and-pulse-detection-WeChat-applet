// app.js
var mqtt = require('/utils/mqtt.min.js')
App({

  globalData: {
    userInfo: null,
    client: {},
    timelist: [],
    Templist: [],
    Sao2list: [],
    Hrlist: [],
  },
  onLaunch() {
    //隐藏系统tabBar
    wx.hideTabBar();
    this.globalData.client = mqtt.connect('wxs://www.codingroad.top:8084/mqtt')
    this.globalData.client.on('connect', (e) => {
      console.log('服务器连接成功')
      this.globalData.client.subscribe('/mqtt/testtopic', function (err) {
        if (!err) {
          console.log('成功订阅设备上行数据topic')
        }
      })
    })
  },



})
