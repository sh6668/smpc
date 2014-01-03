#include "lastfmalbumprovider.h"

LastFMAlbumProvider::LastFMAlbumProvider(QString albumName,QString artistName) : QObject(0) {
    mAlbumName = albumName;
    mArtistName = artistName;

    mXMLNetAccess = 0;
    mAlbumArtAccess = 0;
    mImageData = 0;
    mImageURL = "";
}

LastFMAlbumProvider::~LastFMAlbumProvider() {
    if ( mXMLNetAccess ) {
        disconnect(mXMLNetAccess);
        mXMLNetAccess->deleteLater();
        mXMLNetAccess = 0;
    }
    if ( mAlbumArtAccess ) {
        disconnect(mAlbumArtAccess);
        mAlbumArtAccess->deleteLater();
        mAlbumArtAccess = 0;
    }
}


LastFMAlbumProvider::LastFMAlbumProvider(MpdArtist &artist) {

}

void LastFMAlbumProvider::downloadImageData(QUrl imageURL) {
    mAlbumArtAccess = new QNetworkAccessManager();
    QNetworkRequest downloadRequest(imageURL);
    connect(mAlbumArtAccess,SIGNAL(finished(QNetworkReply*)),this,SLOT(imageDownloaded(QNetworkReply*)));
    mAlbumArtAccess->get(downloadRequest);
}

void LastFMAlbumProvider::xmlDownloaded(QNetworkReply *reply) {
    QByteArray data = reply->readAll();

    // netaccess not needed anymore
    //    delete(mXMLNetAccess);
    //    mXMLNetAccess = 0;

    //    QString outString(data);

    QXmlStreamReader xmlReader(data);
    while ( !xmlReader.atEnd() ) {
        QXmlStreamReader::TokenType token = xmlReader.readNext();
        if ( token == QXmlStreamReader::StartDocument) {
            // nothing important, skip
            continue;
        }
        else if ( token == QXmlStreamReader::StartElement ) {
            // Found some data
            if ( xmlReader.name() == "lfm" ) {
                continue;
            }

            if ( xmlReader.name() == "album" ) {
                parseAlbum(xmlReader);
            }
        }
    }
    if ( mImageURL != "" ) {
        downloadImageData(mImageURL);
    } else {
        emit ready(new AlbumInformation(mAlbumName,mArtistName,0,0));
    }
}

void LastFMAlbumProvider::parseAlbum(QXmlStreamReader &xmlReader) {
    QString albumURL;

    // Security check
    if ( ( xmlReader.tokenType() != QXmlStreamReader::StartElement &&
           xmlReader.name() == "album" )) {
        return;
    }

    bool foundPreferredAlbumArt = false;

    while ( !(xmlReader.tokenType() == QXmlStreamReader::EndElement && xmlReader.name() == "album") ) {
        if ( xmlReader.tokenType() == QXmlStreamReader::StartElement &&
             xmlReader.name() == "image" && !foundPreferredAlbumArt ) {

            // Get image size attribute
            QXmlStreamAttributes attributes = xmlReader.attributes();
            if ( attributes.hasAttribute("size") && attributes.value("size").toString() == COVERSIZE) {
                // Found desired cover size
                xmlReader.readNext();
                if ( xmlReader.tokenType() == QXmlStreamReader::Characters ) {
                    mImageURL = xmlReader.text().toString();
                    foundPreferredAlbumArt = true;
                }
            }
            else {
                xmlReader.readNext();
                if ( xmlReader.tokenType() == QXmlStreamReader::Characters ) {
                    mImageURL = xmlReader.text().toString();
                }
            }
        }

        if ( xmlReader.tokenType() == QXmlStreamReader::StartElement &&
             xmlReader.name() == "wiki" ) {
            parseWikiInformation(xmlReader);
        }

        xmlReader.readNext();
    }

}

void LastFMAlbumProvider::parseWikiInformation(QXmlStreamReader &xmlReader) {
    // Security check
    if ( ( xmlReader.tokenType() != QXmlStreamReader::StartElement &&
           xmlReader.name() == "wiki" )) {
        return;
    }

    while ( !(xmlReader.tokenType() == QXmlStreamReader::EndElement && xmlReader.name() == "wiki") ) {
        if ( xmlReader.tokenType() == QXmlStreamReader::StartElement &&
             xmlReader.name() == "content") {
            xmlReader.readNext();
            if ( xmlReader.tokenType() == QXmlStreamReader::Characters ) {
                mAlbumInfo = xmlReader.text().toString();
            }
        }
        xmlReader.readNext();
    }

}

void LastFMAlbumProvider::startDownload() {
    // Get last.fm album info

    QString lastfmXMLURL = QString("http://ws.audioscrobbler.com/2.0/?method=album.getinfo&api_key=")
            + LASTFMAPIKEY + QString("&artist=") + mArtistName + QString("&album=") + mAlbumName;
    QUrl requestURL(lastfmXMLURL);

    QNetworkRequest downloadRequest(requestURL);
    QNetworkAccessManager *mXMLNetAccess = new QNetworkAccessManager(this);
    if( !mXMLNetAccess->networkAccessible()) {
    }
    else {
    }

    connect(mXMLNetAccess,SIGNAL(finished(QNetworkReply*)),this,SLOT(xmlDownloaded(QNetworkReply*)));
    QNetworkReply *reply = mXMLNetAccess->get(downloadRequest);

    //    for( int i = 0; data.size();i++) {
    //        qDebug() << data.at(i);
    //    }


}

void LastFMAlbumProvider::imageDownloaded(QNetworkReply *reply) {
    QByteArray *imgData = new QByteArray(reply->readAll());
    emit ready(new AlbumInformation(mAlbumName,mArtistName,mImageURL,imgData));
}

QString LastFMAlbumProvider::getImageURL() {
    return "";
}
